################################################################################
# myplot.py
# Consistently formatted publication-quality plots with MATLAB-style syntax.
################################################################################
# Part of the LaTeX IEM Template
# https://git.iem.at/fshstk/latex-iem-template
#
# (c) 2020 Fabian Hummel
# CC BY-SA 4.0 (https://creativecommons.org/licenses/by-sa/4.0/)
################################################################################
# CHEAT SHEET:
# 1. figure()
# 2. <MATLAB-style figure code>
# 3. finalise("filename_no_suffix")
################################################################################

# Imports for plots and common tasks:
import locale
import matplotlib
from matplotlib.pyplot import *

# Locale for comma as decimal separator:
locale.setlocale(locale.LC_NUMERIC, "de_AT")
matplotlib.pyplot.rcParams["axes.formatter.use_locale"] = True

# Plot settings:
matplotlib.pyplot.rcParams["font.size"] = 14
matplotlib.pyplot.rcParams["axes.linewidth"] = 1.5
plotsize = (10, 5)

# Cusom plot functions:
def figure(figsize=plotsize):
    return matplotlib.pyplot.figure(figsize=figsize)


def plot(n1, n2=None, style=None):
    if n2 is not None:
        x = n1
        y = n2
    else:
        x = range(len(n1))
        y = n1

    if style is not None:
        return matplotlib.pyplot.plot(x, y, style, color="0.1")
    else:
        return matplotlib.pyplot.plot(x, y, color="0.1")


def stem(n1, n2=None, marker=None):
    if n2 is not None:
        x = n1
        y = n2
    else:
        x = range(len(n1))
        y = n1
    if marker is not None:
        return matplotlib.pyplot.stem(
            x, y, linefmt="0.1", basefmt="k", markerfmt=marker
        )
    else:
        return matplotlib.pyplot.stem(x, y, linefmt="0.1", basefmt="k", markerfmt="ko")


def specgram(*args, **kwargs):
    return matplotlib.pyplot.specgram(*args, **kwargs, cmap=get_cmap("Greys"))
    # return matplotlib.pyplot.specgram(*args, **kwargs, cmap=get_cmap("gray"))


def finalise(filename=None):
    matplotlib.pyplot.tight_layout()

    if filename is not None:
        matplotlib.pyplot.savefig(
            "../plots/{}.pdf".format(filename), bbox_inches="tight"
        )
