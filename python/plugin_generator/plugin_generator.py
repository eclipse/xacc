import argparse
import os


PLUGIN_TYPES = {"compiler", "irtransformation", "iroptimization", "gate-instruction"}

DESCRIPTION = """
A utility for generating plugin skeletons.
E.g. to generate a new compiler plugin for the "my_lang" language, run
'python3 -m xacc generate-plugin -t compiler -n my_lang'
"""

def add_subparser(subparsers):
    """Add the 'generate-plugin' subparser to subparsers."""
    subparser = subparsers.add_parser("generate-plugin", help="A utility for generating plugin skeletons.", description=DESCRIPTION,
                                      formatter_class=argparse.RawDescriptionHelpFormatter)
    subparser.add_argument("-o", "--output", help="output location (defaults to current directory)")
    subparser.add_argument("-v", "--verbose", help="increase verbosity", action="store_true")
    subparser.add_argument("-l", "--list", help="list available plugin types", action="store_true")
    required_group = subparser.add_argument_group("required arguments")
    required_group.add_argument("-t", "--type", help="plugin type", choices=PLUGIN_TYPES)
    required_group.add_argument("-n", "--name", help="plugin name")


def run_generator(args, xacc_root):
    """Run the appropriate generator given args."""
    if args.list and not (args.type and args.name):
        print("Avalable plugin types:")
        print(PLUGIN_TYPES)
        exit(0)
    if args.type and args.name:
        args.libname = "xacc-{}".format(args.name.lower())
        if args.output:
            os.chdir(args.output)
        os.mkdir(args.libname)
        templates_dir = os.path.join(os.path.dirname(__file__), "templates/")
        output_dir = os.path.join(os.getcwd(), args.libname)
        if args.type.lower() in ["irtransformation","iroptimization"]:
            template_dir = os.path.join(templates_dir, "irtransformation/")
            if args.verbose:
                print("Generating an IR Optimization or Transformation plugin...")
                generate_irtransformation(template_dir, output_dir, xacc_root, args)
        elif args.type.lower() == "compiler":
            template_dir = os.path.join(templates_dir, "compiler/")
            if args.verbose:
                print("Generating a compiler plugin...")
                generate_compiler(template_dir, output_dir, xacc_root, args)
        elif args.type.lower() == "accelerator":
            print('Accelerator plugin gen not implemented yet.')
            return
        elif args.type.lower() == "gate-instruction":
            template_dir = os.path.join(templates_dir, "gate_instruction/")
            if args.verbose:
                print("Generating a compiler plugin...")
                generate_instruction(template_dir, output_dir, xacc_root, args)
    else:
        print("Please specify a type (-t) and name (-n). Use -l (--list) to see available plugins to generate")

def generate(template_dir, output_dir, format_func, verbose=False):
    for dirpath, dirnames, filenames in os.walk(template_dir):
        dir_rel_path = os.path.relpath(dirpath, template_dir)
        if verbose:
            print("Entered {}/".format(dir_rel_path))
        for dirname in dirnames:
            outdir_name = format_func(dirname)
            outdir_path = os.path.join(output_dir, dir_rel_path, outdir_name)
            os.mkdir(outdir_path)
            if verbose:
                print("Created {}".format(outdir_path))
        for filename in filenames:
            with open(os.path.join(dirpath, filename), 'r') as template_file:
                contents = template_file.read()
            outfile_name = format_func(filename)
            outfile_path = os.path.join(output_dir, dir_rel_path, outfile_name)
            contents = format_func(contents)
            with open(outfile_path, 'w') as outfile:
                outfile.write(contents)
            if verbose:
                print("Created {}".format(outfile_path))


def generate_irtransformation(template_dir, output_dir, xacc_root_path, args):
    irt_lib_name = "{}transformation".format(args.libname)
    class_name = "{}IRTransformation".format(args.name.capitalize())
    generate(template_dir, output_dir, format_func=lambda s: s.format(
        xacc_root=xacc_root_path,
        lib_name=args.libname,
        project_name=args.name,
        project_name_upper=args.name.upper(),
        irt_bundle_name=irt_lib_name.replace('-','_'),
        irt_lib_name=irt_lib_name,
        irt_class_name=class_name,
        irt_class_name_lower=class_name.lower(),
        irt_class_name_upper=class_name.upper()
    ), verbose=args.verbose)


def generate_compiler(template_dir, output_dir, xacc_root_path, args):
    compiler_lib_name = "{}compiler".format(args.libname)
    class_name = "{}Compiler".format(args.name.capitalize())
    generate(template_dir, output_dir, format_func=lambda s: s.format(
        xacc_root=xacc_root_path,
        lib_name=args.libname,
        project_name=args.name,
        project_name_upper=args.name.upper(),
        compiler_bundle_name=compiler_lib_name.replace('-', '_'),
        compiler_lib_name=compiler_lib_name,
        compiler_class_name=class_name,
        compiler_class_name_lower=class_name.lower(),
        compiler_class_name_upper=class_name.upper()
    ), verbose=args.verbose)

def generate_instruction(template_dir, output_dir, xacc_root_path, args):
    inst_lib_name = "{}instruction".format(args.libname)
    class_name = "{}".format(args.name.capitalize())
    generate(template_dir, output_dir, format_func=lambda s: s.format(
        xacc_root=xacc_root_path,
        lib_name=args.libname,
        project_name=args.name,
        project_name_upper=args.name.upper(),
        inst_bundle_name=inst_lib_name.replace('-','_'),
        inst_lib_name=inst_lib_name,
        inst_class_name=class_name,
        inst_class_name_lower=class_name.lower(),
        inst_class_name_upper=class_name.upper()
    ), verbose=args.verbose)
