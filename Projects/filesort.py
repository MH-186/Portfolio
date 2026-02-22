import os
import shutil

directory = os.path.join(os.path.expanduser("~"), "Desktop")

extensions = {
    ".jpg": "Images",
    ".png": "Images",
    ".mp4": "Videos",
    ".mp3": "Musics",
    ".doc": "Documents", 
    ".pdf": "Documents", 
    ".txt": "Documents", 
    ".exe": "Shortcuts"
}

for filename in os.listdir(directory):
    file_path = os.path.join(directory, filename)

    if os.path.isfile(file_path):
        extension = os.path.splitext(filename)[1].lower()

        if extension in extensions:
            folder_name = extensions[extension]

            folder_path = os.path.join(directory, folder_name)

            if not os.path.exists(folder_path):
                os.makedirs(folder_path)

            destination_path = os.path.join(folder_path, filename)

            shutil.move(file_path, destination_path)

            print(f"Moved {filename} to {folder_name} folder.")
        else:
            print(f"{filename} has an unknown extension.")
