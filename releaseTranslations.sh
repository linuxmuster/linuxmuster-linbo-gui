#!/bin/bash
TRANSLATIONS=$(ls ./resources/translations/linbo-gui/*-*.ts)

while read lang; do
echo "Releasing $lang"
/home/dorian/Qt/5.15.2/gcc_64/bin/lrelease $lang
done <<< "$TRANSLATIONS"
