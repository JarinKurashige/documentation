#!/usr/bin/env python
"""
Setuptools setup specification for package 'openblt'.
"""
__docformat__ = 'reStructuredText'
# ***************************************************************************************
#  File Name: setup.py
#
# ---------------------------------------------------------------------------------------
#                           C O P Y R I G H T
# ---------------------------------------------------------------------------------------
#    Copyright (c) 2018  by Feaser    http://www.feaser.com    All rights reserved
#
# ---------------------------------------------------------------------------------------
#                             L I C E N S E
# ---------------------------------------------------------------------------------------
# This file is part of OpenBLT. It is released under a commercial license. Refer to
# the license.pdf file for the exact licensing terms. It should be located in
# '.\Doc\license.pdf'. Contact Feaser if you did not receive the license.pdf file.
# 
# In a nutshell, you are allowed to modify and use this software in your closed source
# proprietary application, provided that the following conditions are met:
# 1. The software is not released or distributed in source (human-readable) form.
# 2. These licensing terms and the copyright notice above are not changed.
# 
# This software has been carefully tested, but is not guaranteed for any particular
# purpose. Feaser does not offer any warranties and does not guarantee the accuracy,
# adequacy, or completeness of the software and is not responsible for any errors or
# omissions or the results obtained from use of the software.
#
# ***************************************************************************************


# ***************************************************************************************
#  Imports
# ***************************************************************************************
from setuptools import setup


# ***************************************************************************************
#  Implementation
# ***************************************************************************************
setup ( \
    name = 'openblt',
    version = '1.3.8',
    description = 'Python wrapper for the OpenBLT host library (LibOpenBLT).',
    long_description = open('README.rst', 'r').read(),
    author = 'Frank Voorburg',
    author_email = 'voorburg@feaser.com',
    url = 'https://www.feaser.com/openblt/doku.php?id=manual:libopenblt',
    packages = ['openblt'],
)


# ********************************* end of setup.py *************************************
