#! /bin/bash
# The function name should be given as an argument.

dot -Tpdf cfg.${1}.dot -o cfg.pdf
dot -Tpdf ddg.${1}.dot -o ddg.pdf
dot -Tpdf cdg.${1}.dot -o cdg.pdf
dot -Tpdf mdg.${1}.dot -o mdg.pdf
dot -Tpdf pdg.${1}.dot -o pdg.pdf
dot -Tpdf psg.${1}.dot -o psg.pdf
