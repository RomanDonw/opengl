import sys
import struct
import os

UCMESH_VERSION = 0

UCMESH_PRIMITIVE_TYPE_TRIANGLE = 0
UCMESH_PRIMITIVE_TYPE_QUAD = 1

if len(sys.argv) >= 2 + 1:
    if os.path.isfile(sys.argv[1]):
        if os.path.isfile(sys.argv[2]):
            if input(f"Output file \"{sys.argv[2]}\" already exist. Overwrite it (type \"y\" and press Enter to confirm)? ").lower() != "y":
                print("Overwriting canceled.")
                exit(0)

        with open(sys.argv[1], "r") as f_in:
            vertices_raw_data = []
            uvs_raw_data = []

            next_generated_vertex_info_index = 0
            vertices_info_data = []
            primitives_data = []

            line_num = 0
            for line in f_in.readlines():
                line_num += 1

                options = line.split(" ")
                if len(options) == 0 or options[0] == "#" or options[0] == "\n": continue

                match options[0]:
                    case "v": # vertex.
                        if len(options) < 3 + 1:
                            print(f"[{line_num}]: invalid vertex declaration, option skipped.")
                            continue

                        vertex = []
                        for i in range(3):
                            try:
                                vertex.append(float(options[i + 1]))
                            except TypeError:
                                print(f"[{line_num}]: invalid option's argument value \"{options[i + 1]}\" (argument number {i + 1}; required float), option skipped.")

                        if len(vertex) < 3: continue

                        vertices_raw_data.append(vertex)

                    case "vt": # UV coord.
                        if len(options) < 2 + 1:
                            print(f"[{line_num}]: invalid UV declaration, option skipped.")
                            continue

                        uv = []
                        for i in range(2):
                            try:
                                uv.append(float(options[i + 1]))
                            except TypeError:
                                print(f"[{line_num}]: invalid option's argument value \"{options[i + 1]}\" (argument number {i + 1}; required float), option skipped.")

                        if len(uv) < 2: continue

                        uvs_raw_data.append(uv)

                    case "f": # fragment.
                        if len(options) < 3 + 1:
                            print(f"[{line_num}]: fragment has less than 3 vertices, option skipped.")
                            continue

                        if len(options) > 4 + 1:
                            print(f"[{line_num}]: fragment has more than 4 vertices, that types of fragments is unsupported. Option skipped.")
                            continue

                        #vertex_indices = []
                        #uv_indices = []


                        # === Read fragment. ===

                        verts_info = []
                        flag_continue = False
                        for option_arg in options[1:]:
                            vertex_index = -1
                            uv_index = -1

                            sub_options = option_arg.split("/")
                            if len(sub_options) == 0:
                                print(f"[{line_num}]: invalid argument syntax for fragment option, skipped.")
                                flag_continue = True
                                break

                            elif len(sub_options) == 1:
                                try:
                                    vertex_index = int(sub_options[0]) - 1

                                    if vertex_index < 0:
                                        print(f"[{line_num}]: subargument type must be positive integer that greater than zero. Option skipped.")
                                        raise RuntimeError()
                                    
                                    elif vertex_index >= len(vertices_raw_data):
                                        print(f"[{line_num}]: vertex index out of vertices array bounds, skipped.")
                                        raise RuntimeError()
                                    
                                except TypeError:
                                    print(f"[{line_num}]: invalid subargument type (required int). Option skipped.")
                                    flag_continue = True
                                    break

                                except RuntimeError:
                                    flag_continue = True
                                    break

                            elif len(sub_options) >= 2:
                                try:
                                    vertex_index = int(sub_options[0]) - 1
                                    uv_index = int(sub_options[1]) - 1

                                    if vertex_index < 0 or uv_index < 0:
                                        print(f"[{line_num}]: subargument type must be positive integer that greater than zero. Option skipped.")
                                        raise RuntimeError()
                                    
                                    elif vertex_index >= len(vertices_raw_data):
                                        print(f"[{line_num}]: vertex index out of vertices array bounds, skipped.")
                                        raise RuntimeError()
                                    
                                    elif uv_index >= len(uvs_raw_data):
                                        print(f"[{line_num}]: UV index out of UVs array bounds, skipped.")
                                        raise RuntimeError()
                                    
                                except TypeError:
                                    print(f"[{line_num}]: invalid subargument type (required int). Option skipped.")
                                    flag_continue = True
                                    break

                                except RuntimeError:
                                    flag_continue = True
                                    break

                            
                            if vertex_index < 0:
                                print(f"[{line_num}]: occured unexpected error while parsing fragment, skipped.")
                                flag_continue = True
                                break

                            if uv_index < 0:
                                verts_info.append({"vertex": vertices_raw_data[vertex_index], "UV": [0.0, 0.0]})
                            else:
                                verts_info.append({"vertex": vertices_raw_data[vertex_index], "UV": uvs_raw_data[uv_index]})
                        
                        if flag_continue: continue


                        primitive = []
                        for vrtx in verts_info:
                            try: # already has this vertex in array.
                                idx = vertices_info_data.index(vrtx)
                                primitive.append(idx)

                            except ValueError: # it's a new vertex, need to add to array.
                                vertices_info_data.append(vrtx)
                                primitive.append(next_generated_vertex_info_index)
                                next_generated_vertex_info_index += 1

                        primitives_data.append(primitive)

                    case _:
                        print(f"[{line_num}]: unsupported/unknown option \"{options[0]}\", skipped.")
                        continue

            with open(sys.argv[2], "wb") as f_out:
                f_out.write(struct.pack("6s", b"UCMESH"))
                f_out.write(struct.pack("<H", UCMESH_VERSION))

                f_out.write(struct.pack("<LL", len(vertices_info_data), len(primitives_data)))

                for vertex_data in vertices_info_data:
                    vertex = vertex_data["vertex"]
                    uv = vertex_data["UV"]

                    f_out.write(struct.pack("<5f", vertex[0], vertex[1], vertex[2], uv[0], uv[1]))

                for primitive in primitives_data:
                    match len(primitive):
                        case 3:
                            f_out.write(struct.pack("<B3L", UCMESH_PRIMITIVE_TYPE_TRIANGLE, primitive[0], primitive[1], primitive[2]))

                        case 4:
                            f_out.write(struct.pack("<B4L", UCMESH_PRIMITIVE_TYPE_QUAD, primitive[0], primitive[1], primitive[2], primitive[3]))

                        case _:
                            print(f"unsupported primitive type in UCMESH (indices count: {len(primitive)})")

    else:
        print(f"Input file \"{sys.argv[1]}\" doesn't exist.")

else:
    print(f"Usage: {sys.argv[0]} <input .obj file> <output UCMESH file>")