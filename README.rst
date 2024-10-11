===========
time64-prep
===========

Overview
========
time64-prep is a tool to provide a more secure transition path for
rebuilding the systems using 32-bit ``time_t`` (time32 ABI) to
the 64-bit variant of this type (time64 ABI).

Note that this tool is not technically required to perform
the transition.  In fact, an even better approach is to build new
binaries in a separate root, then merge them into the main system;
or at least use a separate root to prepare binary packages for
the transition.  However, when transition in place is intended,
time64-prep can reduce the risk of time32 and time64 ABI mixing between
different binaries, that could lead to catastrophic and potentially
exploitable memory errors.

To achieve this, time64-prep creates mirror ``libt32`` library
directories containing copies of the shared libraries from ``lib``,
and patches all existing binaries to list these directories in their
RUNPATHs.  As libraries are rebuilt to use 64-bit ``time_t``
and installed into ``lib``, old executables (still using time32 ABI)
continue using the libraries from ``libt32`` — until they themselves
are rebuilt and therefore lose the injected RUNPATH.

Note that this is not a perfect solution.  In particular, ecosystems
such as Perl and Python install loadable modules in non-standard
locations and they will suffer from mixed ABIs nevertheless.


Dependencies
============
time64-prep requires the following Python packages:
- ``portage`` to obtain the list of installed files
- ``pyelftools`` to inspect binaries (and gather their RUNPATHs)

Additionally, it requires the following executables:
- ``ldconfig`` to find library directories
- ``patchelf`` to modify binary RUNPATHs


Usage
=====
By default, time64-prep runs in a "pretend" mode, in which it prints
the operations that would be performed without actually performing them.
Ideally, do a such run first and inspect the output for any
irregularities::

    time64-prep

If everything looks fine, you can pass ``-u`` to apply the updates::

    time64-prep -u

It also accepts a ``--prefix`` argument to perform the operation
in the specified system root, instead of ``/``::

    time64-prep --prefix /mnt/gentoo

After performing the update, the system can be transitioned to time64
per Gentoo instructions.  Once done, the created ``libt32`` directories
should be removed manually.

If errors occur while patching RUNPATHs, the tool will terminate early,
without actually copying libraries.  It can be afterwards run again
to attempt to patch the remaining binaries, and possibly copy libraries
afterwards.

If errors occur while copying or moving libraries, the tool attempts
to copy/move as many as possible.  It dos not support safely resuming
at this point, and any failures should be fixed manually.


Expert usage
============
By default, time64-prep copies all shared libraries and moves static
libraries (to reduce the risk of mixing ABIs while statically linking).
This implies that depending on package ordering, and particularly when
cyclic dependencies are involved, executables may end up temporarily
mixing time32 and time64 ABI.

Alternatively, ``--mv-shared-libs`` option can be used to move shared
libraries instead of copying them.  This reduces the risk of ABI mixing,
at the cost of making the rebuilds involving cyclic dependencies fail.
