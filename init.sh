#!/bin/bash

# Run this before starting work on your project.
#
# This script replaces all occurrences of "NewProject" with a user-given name.
# It also generates a new six-digit UUID for the <JUCERPROJECT> tag inside the
# *.jucer settings file. This is needed since duplicate project IDs can lead to
# problems, at least with plug-ins.

PROJECTNAME=""

echo "What is your project called? Must contain only letters."
echo "(It should also be in PascalCase to comply with C++ class names.)"

while test -z "${PROJECTNAME}"; do
  echo -n "Project Name: "
  read PROJECTNAME

  if [[ "${PROJECTNAME}" =~ [^a-zA-Z] ]]; then
    echo "Project name must contain only letters!"
    PROJECTNAME=""
  fi
done

UUID=$(LC_ALL=C tr -dc 'a-zA-Z0-9' < /dev/urandom | head -c 6)
echo "New project ID generated: $UUID"

echo -n "Changing UUID in NewProject.jucer ..."
sed -i '' 's/JUCERPROJECT\ id="......"/JUCERPROJECT\ id="dhaha"/g' NewProject.jucer
echo " Done"

echo -n "Replacing ALL occurrences of NewProject with $PROJECTNAME ..."
sed -i "" "s/NewProject/$PROJECTNAME/g" NewProject.jucer
find .vscode -type f -print0 | xargs -0 sed -i "" "s/NewProject/$PROJECTNAME/g"
find Source -type f -print0 | xargs -0 sed -i "" "s/NewProject/$PROJECTNAME/g"
echo " Done"

echo -n "Renaming NewProject.jucer to $PROJECTNAME.jucer ..."
mv NewProject.jucer $PROJECTNAME.jucer
echo " Done"
