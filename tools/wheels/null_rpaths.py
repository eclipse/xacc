import os,sys,pkg_resources
import re
from os.path import isfile, exists, abspath, basename
import shutil
import itertools
import functools
import argparse
from auditwheel.policy import (load_policies, get_policy_name, get_priority_by_name,
                     POLICY_PRIORITY_HIGHEST)
from os.path import exists, relpath, dirname, basename, abspath, isabs
from os.path import join as pjoin
from subprocess import check_call, check_output, CalledProcessError
from distutils.spawn import find_executable
from typing import Optional
from os.path import join as pjoin

from auditwheel.policy import get_replace_platforms
from auditwheel.wheeltools import InWheelCtx, add_platforms
from auditwheel.wheel_abi import get_wheel_elfdata
from auditwheel.elfutils import elf_read_rpaths, is_subdir, elf_read_dt_needed
from auditwheel.hashfile import hashfile
from auditwheel.repair import patchelf_set_rpath, verify_patchelf, copylib

def xacc_repair_wheel(wheel_path: str, abi: str, lib_sdir: str, out_dir: str,
                 update_tags: bool) -> Optional[str]:

    external_refs_by_fn = get_wheel_elfdata(wheel_path)[1]
    soname_map = {}  # type: Dict[str, str]
    if not isabs(out_dir):
        out_dir = abspath(out_dir)

    wheel_fname = basename(wheel_path)

    with InWheelCtx(wheel_path) as ctx:
        ctx.out_wheel = pjoin(out_dir, wheel_fname)

        if update_tags:
            ctx.out_wheel = add_platforms(ctx, [abi],
                                          get_replace_platforms(abi))
    return ctx.out_wheel

def configure_parser(sub_parsers):
    policy_names = [p['name'] for p in load_policies()]
    highest_policy = get_policy_name(POLICY_PRIORITY_HIGHEST)
    help = "Vendor in external shared library dependencies of a wheel."
    p = sub_parsers.add_parser('repair', help=help, description=help)
    p.add_argument('WHEEL_FILE', help='Path to wheel file.')
    p.add_argument('-f',
                   '--force',
                   help='Override symbol version ABI check',
                   action='store_true')
    p.add_argument(
        '--plat',
        dest='PLAT',
        help='Desired target platform. (default: "%s")' % highest_policy,
        choices=policy_names,
        default=highest_policy)
    p.add_argument('-L',
                   '--lib-sdir',
                   dest='LIB_SDIR',
                   help=('Subdirectory in packages to store copied libraries.'
                         ' (default: ".libs")'),
                   default='.libs')
    p.add_argument('-w',
                   '--wheel-dir',
                   dest='WHEEL_DIR',
                   type=abspath,
                   help=('Directory to store delocated wheels (default:'
                         ' "wheelhouse/")'),
                   default='wheelhouse/')
    p.add_argument('--no-update-tags',
                   dest='UPDATE_TAGS',
                   action='store_false',
                   help=('Do not update the wheel filename tags and WHEEL info'
                         ' to match the repaired platform tag.'),
                   default=True)
    p.set_defaults(func=execute)


def execute(args, p):
    import os
    from distutils.spawn import find_executable
    from auditwheel.wheel_abi import analyze_wheel_abi

    if not isfile(args.WHEEL_FILE):
        p.error('cannot access %s. No such file' % args.WHEEL_FILE)
    if find_executable('patchelf') is None:
        p.error('cannot find the \'patchelf\' tool, which is required')

    print('Repairing %s' % basename(args.WHEEL_FILE))

    if not exists(args.WHEEL_DIR):
        os.makedirs(args.WHEEL_DIR)

    wheel_abi = analyze_wheel_abi(args.WHEEL_FILE)
    reqd_tag = get_priority_by_name(args.PLAT)

    if (reqd_tag > get_priority_by_name(wheel_abi.sym_tag)):
        msg = ('cannot repair "%s" to "%s" ABI because of the presence '
               'of too-recent versioned symbols. You\'ll need to compile '
               'the wheel on an older toolchain.' %
               (args.WHEEL_FILE, args.PLAT))
        p.error(msg)

    if (reqd_tag > get_priority_by_name(wheel_abi.ucs_tag)):
        msg = ('cannot repair "%s" to "%s" ABI because it was compiled '
               'against a UCS2 build of Python. You\'ll need to compile '
               'the wheel against a wide-unicode build of Python.' %
               (args.WHEEL_FILE, args.PLAT))
        p.error(msg)

    out_wheel = xacc_repair_wheel(args.WHEEL_FILE,
                             abi=args.PLAT,
                             lib_sdir=args.LIB_SDIR,
                             out_dir=args.WHEEL_DIR,
                             update_tags=args.UPDATE_TAGS)

    if out_wheel is not None:
        print('\nFixed-up wheel written to %s' % out_wheel)

def main():
    dist = pkg_resources.get_distribution('auditwheel')
    version = 'auditwheel %s installed at %s (python %s)' % (
        dist.version, dist.location, sys.version[:3])

    p = argparse.ArgumentParser(description='Fix XACC Linux Wheel.')
    p.set_defaults(prog=os.path.basename(sys.argv[0]))
    p.add_argument('-V', '--version', action='version', version=version)
    p.add_argument("-v",
                   "--verbose",
                   action='count',
                   dest='verbose',
                   default=0,
                   help='Give more output. Option is additive')
    sub_parsers = p.add_subparsers(metavar='command', dest='cmd')

    configure_parser(sub_parsers)

    args = p.parse_args()

    if not hasattr(args, 'func'):
        p.print_help()
        return

    try:
        rval = args.func(args, p)
    except:
        # TODO(rmcgibbo): nice message
        raise

    return rval

if __name__ == '__main__':
    sys.exit(main())
