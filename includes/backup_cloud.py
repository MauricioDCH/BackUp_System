# pip install --upgrade google-auth google-auth-oauthlib google-api-python-client

import sys
import os
import io
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload, MediaIoBaseDownload

# === Ámbito de permisos para acceso a Drive ===
SCOPES = ['https://www.googleapis.com/auth/drive.file']

# === Ruta absoluta del directorio donde está el script ===
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# === Archivos de credenciales ===
credentials_path = os.path.join(BASE_DIR, 'ignorar', 'credentials.json')
token_path = os.path.join(BASE_DIR, 'ignorar', 'token.json')


def authenticate():
    """
    Autenticación con OAuth 2.0 usando token.json o credentials.json.
    """
    creds = None
    if os.path.exists(token_path):
        creds = Credentials.from_authorized_user_file(token_path, SCOPES)
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            creds.refresh(Request())
        else:
            flow = InstalledAppFlow.from_client_secrets_file(credentials_path, SCOPES)
            creds = flow.run_console()
        with open(token_path, 'w') as token:
            token.write(creds.to_json())
    return build('drive', 'v3', credentials=creds)

def create_drive_folder(service, name, parent_id=None):
    """
    Crea una carpeta en Google Drive. Si se da un `parent_id`, será su carpeta contenedora.
    """
    metadata = {
        'name': name,
        'mimeType': 'application/vnd.google-apps.folder'
    }
    if parent_id:
        metadata['parents'] = [parent_id]
    folder = service.files().create(body=metadata, fields='id').execute()
    return folder.get('id')

def upload_file(service, filepath, parent_id):
    """
    Sube un archivo a Google Drive dentro de `parent_id`.
    """
    file_metadata = {
        'name': os.path.basename(filepath),
        'parents': [parent_id]
    }
    media = MediaFileUpload(filepath, resumable=True)
    file = service.files().create(body=file_metadata, media_body=media, fields='id').execute()
    print(f'Archivo subido: {filepath} → ID: {file.get("id")}')

def upload_folder(service, local_folder, parent_drive_id):
    """
    Sube una carpeta completa, incluyendo subcarpetas y archivos.
    """
    folder_name = os.path.basename(local_folder.rstrip('/\\'))
    root_drive_folder_id = create_drive_folder(service, folder_name, parent_drive_id)
    print(f'Carpeta raíz creada en Drive: {folder_name} (ID: {root_drive_folder_id})')

    folder_map = {os.path.abspath(local_folder): root_drive_folder_id}

    for root, dirs, files in os.walk(local_folder):
        current_drive_folder_id = folder_map[os.path.abspath(root)]

        for dir_name in dirs:
            local_path = os.path.join(root, dir_name)
            folder_id = create_drive_folder(service, dir_name, current_drive_folder_id)
            folder_map[os.path.abspath(local_path)] = folder_id
            print(f'Carpeta creada: {dir_name} (ID: {folder_id})')

        for filename in files:
            file_path = os.path.join(root, filename)
            upload_file(service, file_path, current_drive_folder_id)

def download_file(service, file_id, local_path):
    """
    Descarga un archivo desde Google Drive a una ruta local.
    """
    request = service.files().get_media(fileId=file_id)
    fh = io.FileIO(local_path, 'wb')
    downloader = MediaIoBaseDownload(fh, request)
    done = False
    while not done:
        status, done = downloader.next_chunk()
        print(f"Descargando {local_path}: {int(status.progress() * 100)}%")

def list_folder_contents(service, folder_id):
    """
    Lista los contenidos (archivos y subcarpetas) de una carpeta en Google Drive.
    """
    query = f"'{folder_id}' in parents and trashed = false"
    results = service.files().list(q=query, fields="files(id, name, mimeType)").execute()
    return results.get('files', [])

def download_folder(service, folder_id, local_dir):
    """
    Descarga una carpeta completa desde Google Drive a `local_dir`, respetando la estructura.
    """
    if not os.path.exists(local_dir):
        os.makedirs(local_dir)

    contents = list_folder_contents(service, folder_id)
    for item in contents:
        item_name = item['name']
        item_id = item['id']
        item_type = item['mimeType']
        local_path = os.path.join(local_dir, item_name)

        if item_type == 'application/vnd.google-apps.folder':
            print(f"Creando carpeta local: {local_path}")
            download_folder(service, item_id, local_path)
        else:
            print(f"Descargando archivo: {item_name} → {local_path}")
            download_file(service, item_id, local_path)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Uso:")
        print("  python3 backup_cloud.py [archivo|carpeta] ID_DRIVE_DESTINO  # SUBIDA")
        print("  python3 backup_cloud.py download:/ruta/local ID_DRIVE_ORIGEN  # DESCARGA")
        sys.exit(1)

    local_path = sys.argv[1]
    drive_id = sys.argv[2]

    service = authenticate()

    if local_path.startswith("download:"):
        # === Modo descarga ===
        local_dest = local_path.replace("download:", "")
        file = service.files().get(fileId=drive_id, fields="id, name, mimeType").execute()
        mime = file['mimeType']

        if mime == 'application/vnd.google-apps.folder':
            download_folder(service, drive_id, local_dest)
        else:
            filename = file['name']
            download_file(service, drive_id, os.path.join(local_dest, filename))

    else:
        # === Modo subida ===
        if os.path.isfile(local_path):
            upload_file(service, os.path.abspath(local_path), drive_id)
        elif os.path.isdir(local_path):
            upload_folder(service, os.path.abspath(local_path), drive_id)
        else:
            print("Error: La ruta indicada no es un archivo ni una carpeta válida.")
            sys.exit(1)
