import argparse
import os


PLUGIN_TYPES = {"pluginproject", "compiler"}

DESCRIPTION = """
A utility for generating plugin skeletons.

Run 'python3 pyxacc.py generate-plugin -t pluginproject -n pluginname' to generate
the base plugin project. Then, for example, to generate a compiler, cd into the
plugin project and run 'python3 pyxacc.py generate-plugin -t compiler -n TestCompiler'.
"""


def add_subparser(subparsers):
    """Add the 'generate-plugin' subparser to subparsers."""
    subparser = subparsers.add_parser("generate-plugin", help="A utility for generating plugin skeletons.", description=DESCRIPTION,
                                      formatter_class=argparse.RawDescriptionHelpFormatter)
    subparser.add_argument("-l", "--libname",
                           help="library name (e.g. xacc-quilcompiler, which would be the default if run with '-n QuilCompiler')")

    required_group = subparser.add_argument_group("required arguments")
    required_group.add_argument("-t", "--type", help="plugin type", required=True, choices=PLUGIN_TYPES)
    required_group.add_argument("-n", "--name", help="plugin name (e.g. QuilCompiler), or in the case of pluginproject, the project name (e.g. quil)", required=True)


def run_generator(args):
    """Run the appropriate generator given args."""
    if not args.libname:
        args.libname = "xacc-{}".format(args.name.lower())
    os.mkdir(args.libname)
    templates_dir = os.path.join(os.path.dirname(__file__), "plugin_templates/")
    output_dir = os.path.join(os.getcwd(), args.libname)

    if args.type.lower() == "pluginproject":
        template_dir = os.path.join(templates_dir, "pluginproject/")
        generate_project(template_dir, output_dir, args)
    elif args.type.lower() == "compiler":
        template_dir = os.path.join(templates_dir, "compiler/")
        generate_compiler(template_dir, output_dir, args)


def generate(template_dir, output_dir, format_func):
    for dirpath, dirnames, filenames in os.walk(template_dir):
        dir_rel_path = os.path.relpath(dirpath, template_dir)
        for dirname in dirnames:
            outdir_name = format_func(dirname)
            os.mkdir(os.path.join(output_dir, dir_rel_path, outdir_name))
        for filename in filenames:
            with open(os.path.join(dirpath, filename), 'r') as template_file:
                contents = template_file.read()
            outfile_name = format_func(filename)
            contents = format_func(contents)
            with open(os.path.join(output_dir, dir_rel_path, outfile_name), 'w') as outfile:
                outfile.write(contents)


def generate_project(template_dir, output_dir, args):
    generate(template_dir, output_dir, format_func=lambda s: s.format(
        project_name=args.libname,
        project_shortname_upper=args.name.upper(),
        project_shortname_lower=args.name.lower()
    ))


def generate_compiler(template_dir, output_dir, args):
    generate(template_dir, output_dir, format_func=lambda s: s.format(
        lib_name=args.libname,
        class_name=args.name,
        class_name_lower=args.name.lower(),
        class_name_upper=args.name.upper()
    ))
