import argparse
import os


PLUGIN_TYPES = {"pluginproject", "compiler"}

DESCRIPTION = """
A utility for generating plugin skeletons.

E.g. to generate a plugin project for the "quil" language, run
'python3 pyxacc.py generate-plugin -t pluginproject -n quil' to generate a plugin project, to which you can 
add plugins. 

E.g. to generate a compiler plugin, run 'python3 pyxacc.py generate-plugin -t compiler -n quil'.
"""


def add_subparser(subparsers):
    """Add the 'generate-plugin' subparser to subparsers."""
    subparser = subparsers.add_parser("generate-plugin", help="A utility for generating plugin skeletons.", description=DESCRIPTION,
                                      formatter_class=argparse.RawDescriptionHelpFormatter)
    subparser.add_argument("-l", "--libname",
                           help="library name (e.g. xacc-quil, which would be the default if run with '-n Quil')")
    subparser.add_argument("-o", "--output", help="output location (defaults to current directory)")
    subparser.add_argument("-v", "--verbose", help="increase verbosity", action="store_true")

    required_group = subparser.add_argument_group("required arguments")
    required_group.add_argument("-t", "--type", help="plugin type", required=True, choices=PLUGIN_TYPES)
    required_group.add_argument("-n", "--name", help="project name (e.g. quil)", required=True)


def run_generator(args):
    """Run the appropriate generator given args."""
    if not args.libname:
        args.libname = "xacc-{}".format(args.name.lower())
    if args.output:
        os.chdir(args.output)
    os.mkdir(args.libname)
    templates_dir = os.path.join(os.path.dirname(__file__), "plugin_templates/")
    output_dir = os.path.join(os.getcwd(), args.libname)

    if args.type.lower() == "pluginproject":
        template_dir = os.path.join(templates_dir, "pluginproject/")
        if args.verbose:
            print("Generating a plugin project...")
        generate_project(template_dir, output_dir, args)
    elif args.type.lower() == "compiler":
        template_dir = os.path.join(templates_dir, "compiler/")
        if args.verbose:
            print("Generating a compiler plugin...")
        generate_compiler(template_dir, output_dir, args)


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


def generate_project(template_dir, output_dir, args):
    generate(template_dir, output_dir, format_func=lambda s: s.format(
        lib_name=args.libname,
        lib_name_alpha="".join(filter(str.isalpha, args.libname)),
        project_name=args.name,
        project_name_upper=args.name.upper()
    ), verbose=args.verbose)


def generate_compiler(template_dir, output_dir, args):
    compiler_lib_name = "{}compiler".format(args.libname)
    class_name = "{}Compiler".format(args.name.capitalize())
    generate(template_dir, output_dir, format_func=lambda s: s.format(
        lib_name=args.libname,
        project_name=args.name,
        project_name_upper=args.name.upper(),
        compiler_bundle_name=compiler_lib_name.replace('-', '_'),
        compiler_lib_name=compiler_lib_name,
        compiler_class_name=class_name,
        compiler_class_name_lower=class_name.lower(),
        compiler_class_name_upper=class_name.upper()
    ), verbose=args.verbose)
