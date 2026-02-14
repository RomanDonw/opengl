from json import JSONDecoder
import sys
import os
import shutil
import subprocess

if len(sys.argv) >= (1) + 1:
    if os.path.isfile(sys.argv[1]):
        with open(sys.argv[1], "r") as f:
            dec = JSONDecoder()
            info = dec.decode(f.read())

            compiler_info = info["compiler"]

            compiler_command = compiler_info["command"]
            compiler_options = compiler_info["options"]
            compiler_include_pathes = compiler_info["include-pathes"]

            linker_info = info["linker"]

            linker_command = linker_info["command"]
            linker_options = linker_info["options"]
            linker_libs_pathes = linker_info["libraries-pathes"]
            linker_static_libs_pathes = linker_info["static-libraries-files-pathes"]
            linker_libraries = linker_info["libraries"]
            output_file_path = linker_info["output-file-path"]

            general_info = info["general"]

            general_tempfolder = general_info["temporary-folder"]
            copy_files = general_info["copy-files"]
            copy_folders = general_info["copy-folders"]
            copy_dest_folder = general_info["copy-destination-folder"]

            project_info = info["project"]

            project_files = project_info["files"]

            if os.path.isdir(general_tempfolder):
                print(f"Building general error: temporary folder \"{general_tempfolder}\" already exist any may be contains neccessary user files. Please change temporary folder or manually delete exist.")
                exit(1)

            compiler_cmd = f"{compiler_command} {compiler_options} "

            for inclfold in compiler_include_pathes:
                if os.path.isdir(inclfold):
                    compiler_cmd += f" -I\"{inclfold}\""
                else:
                    print(f"Warning: includes folder \"{inclfold}\" doesn't exist, adding to list skipped.")

            linker_cmd = f"{linker_command} {linker_options}"

            for libfold in linker_libs_pathes:
                if os.path.isdir(libfold):
                    linker_cmd += f" -L\"{libfold}\""
                else:
                    print(f"Warning: libraries folder \"{libfold}\" doesn't exist, adding to list skipped.")

            linker_cmd_statlibfiles = ""
            for statlibfile in linker_static_libs_pathes:
                if os.path.isfile(statlibfile):
                    linker_cmd_statlibfiles += f" \"{statlibfile}\""
                else:
                    print(f"Warning: specified static library \"{statlibfile}\" doesn't exist, adding to list skipped. This may be cause linking errors in your project.")


            linker_cmd_libsopts = ""
            for libopt in linker_libraries:
                linker_cmd_libsopts += f" -l{libopt}"

            
            # store only verifed source files.
            build_files = []
            for fl in project_files:
                if os.path.isfile(fl):
                    build_files.append(fl)
                else:
                    print(f"Warning: specified file to build \"{fl}\" doesn't exist. This may be cause error on linking project.")

            # create temp folder.
            os.mkdir(general_tempfolder)
            print("Created temporary folder.")

            # build files and correct linker cmd line options.
            linker_cmd_objfiles = ""
            for fl in build_files:
                print(f"Compiling file \"{fl}\"...")

                outfile = os.path.join(general_tempfolder, os.path.basename(fl) + ".o")

                result = os.system(compiler_cmd + f" \"{fl}\" -o \"{outfile}\"")
                if result != 0 or not os.path.isfile(outfile):
                    print(f"Error compiling \"{fl}\" file. Building halted.")

                    print("Removed temporary folder.")
                    shutil.rmtree(general_tempfolder)
                    exit(1)

                linker_cmd_objfiles += f" \"{outfile}\""


            build_folder = os.path.dirname(output_file_path)
            if not os.path.isdir(build_folder):
                os.makedirs(build_folder)
                print("Created tree of build folder.")
            
            print(f"Linking \"{output_file_path}\"...")
            
            linker_cmd = f"{linker_cmd} {linker_cmd_objfiles} {linker_cmd_statlibfiles} {linker_cmd_libsopts} -o {output_file_path}"
            #print(linker_cmd)
            os.system(linker_cmd)


            print("Removed temporary folder.")
            shutil.rmtree(general_tempfolder)

            for fl in copy_files:
                if os.path.isfile(fl):
                    shutil.copy(fl, os.path.join(build_folder, os.path.basename(fl)))
                    print(f"Copied file \"{fl}\" to build folder \"{build_folder}\".")
                else:
                    print(f"Warning: specified file to copy \"{fl}\" doesn't exist, coping skipped.")

            for fold in copy_folders:
                if os.path.isdir(fold):
                    shutil.copytree(fold, os.path.join(build_folder, fold))
                    print(f"Copied folder \"{fold}\" to build folder \"{build_folder}\".")
                else:
                    print(f"Warning: specified folder to copy \"{fl}\" doesn't exist, coping skipped.")

            print("Running output executable...")
            #os.system(f"\"{output_file_path}\"")
            #print(output_file_path)

            #currdir = os.getcwd()
            #os.chdir(build_folder)
            #os.system(os.path.basename(output_file_path))
            #os.chdir(currdir)

            subprocess.Popen([os.path.realpath(output_file_path)], cwd=build_folder)

            print("Done.")

    else:
        print("Specified build info file doesn't exist.")

else:
    print("Usage: ")
    print(f"    {sys.argv[0]} <build info .json file path>")