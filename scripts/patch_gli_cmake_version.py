#Patches the cmake_minimum_required command in a CMakeLists.txt file
#If the command is contained in a comment, it is ignored

import re
import sys

def get_arg(version_pattern):
    required_major = 3
    required_minor = 5
    required_patch = 0
    if len(sys.argv) == 1:
        print("No version argument provided. Defaulting to 3.5")
    elif len(sys.argv) == 2:
        match = version_pattern.search(sys.argv[1])
        if match:
            required_major = int(match.group(1))
            required_minor = int(match.group(2))
            required_patch = int(match.group(3)) if match.group(3) is not None else 0
        else:
            print("Version argument provided is not of form major.minor[.patch]. Defaulting to 3.5")
    else:
        print("Too many arguments passed. The script takes a single version number of the form major.minor[.patch].")
    return required_major, required_minor, required_patch

def patch_file(command_pattern, version_pattern, target_major, target_minor, target_patch):
    with open("CMakeLists.txt", 'r+') as file:  
        current_major = None
        current_minor = None
        current_patch = None
        patch_lines = []
        original_lines = []
        finish_reading = False

        for line in file: 
            if finish_reading:
                original_lines.append(line.strip())
                if line == "\n": print("newline line")
                continue
            match = command_pattern.search(line) 
            if match and match.group(1)[0] != "#":
                print(f"Found command: \"{match.group(0).strip()}\"") 
                current_major = int(match.group(2))
                current_minor = int(match.group(3))
                current_patch = int(match.group(4)) if match.group(4) is not None else 0
                original_lines.append(match.group(0))
                patch_lines.append(
                    f"{match.group(1)}{str(target_major)}.{str(target_minor)}.{str(target_patch)} {match.group(5).strip()}"
                )
                #remove excess spaces, which may improve odds of producing a smaller patch
                patch_lines[-1] = re.sub(r"\s+", " ", patch_lines[-1]) 
                if len(line) < len(patch_lines[-1]):
                    finish_reading = True
                    print("Smaller patch not possible. Rewriting whole file.")
                elif len(patch_lines[-1]) <= len(line):
                    patch_lines[-1] += " " * (len(line) - len(patch_lines[-1]))
                    print(f"Only the first {len(patch_lines)} lines need to be rewritten.")
                    break
            else:
                patch_lines.append(line)
                original_lines.append(line)
        if current_major is None:
            print("Error: the command 'cmake_minimum_required' was not found in CMakeLists.txt")
            print("This is a regular expression error, so its possible the script has failed and the command is present.")
            return
        if (current_major > target_major or \
        (current_major == target_major and current_minor > target_minor) or \
        (current_major == target_major and current_minor == target_minor and current_patch >= target_patch)): 
            print(f"Current CMake version ({current_major}.{current_minor}.{current_patch}) is not less than the target ({target_major}.{target_minor}.{target_patch}). No changes made.") 
        else: 
            if finish_reading: # the length of patched last line < the length of the original last line 
                patch_lines += original_lines[len(patch_lines):] # so rewrite the entire file
            patch_text = "\n".join(patch_lines)
            file.seek(0) 
            file.write(patch_text)
            print("CMakeLists.txt has been patched.")

if __name__ == "__main__":
    #command_pattern = re.compile(r'(#.*)*(cmake_minimum_required\s*\(VERSION\s+)(\d+)\.(\d+)(\.\d+)*(.*\))')
    command_pattern = re.compile(r'(cmake_minimum_required\s*\(\s*VERSION\s+)(\d+)\.(\d+)\.*(\d+)*(.*\)\s*)')
    #version isn't necessarily upper case
    version_pattern = re.compile(r'(\d+)\.(\d+)\.*(\d+)*')

    target_major, target_minor, target_patch = get_arg(version_pattern)
    print(f"Target version: Major:{target_major}, Minor:{target_minor}, Patch:{target_patch}.")
    patch_file(command_pattern, version_pattern, target_major, target_minor, target_patch)