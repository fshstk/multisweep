#!/bin/bash

# Run this before starting work on your project.
#
# This script replaces all occurrences of "NewProject" with a user-given name.
# It also generates a new six-digit UUID for the <JUCERPROJECT> tag inside the
# *.jucer settings file. This is needed since duplicate project IDs can lead to
# problems, at least with plug-ins.

PROJECTDIR=$(pwd)

echo "Please navigate to your project directory before running this script."
echo -n "Current directory: "
echo $PROJECTDIR

echo "----------------------------------------------------------------"

PROJECTNAME=""

echo "What is your project called? Must contain only letters."
echo "(It should also be in PascalCase to comply with C++ class names.)"

while [ -z "$PROJECTNAME" ]; do
  echo -n "Project Name: "
  read PROJECTNAME

  # false positive when trailing space
  if [[ "${PROJECTNAME}" =~ [^a-zA-Z] ]]; then
    echo "Project name must contain only letters!"
    PROJECTNAME=""
  fi
done

echo "----------------------------------------------------------------"

echo "Where is your JUCE library directory?"
echo "(The one that contains the Projucer application and the modules directory.)"
echo "Leave blank for default: ~/JUCE"

echo -n "Project Directory: "
read JUCEDIR

while [ ! -d "$JUCEDIR" ] && [ ! -z "$JUCEDIR" ]; do
  echo "Not a valid directory."
  echo -n "Project Directory: "
  read JUCEDIR
done

echo "----------------------------------------------------------------"

UUID=$(LC_ALL=C tr -dc 'a-zA-Z0-9' < /dev/urandom | head -c 6)
echo "New project ID generated: $UUID"

echo "----------------------------------------------------------------"

echo "Changing UUID in NewProject.jucer ..."
sed -i '' 's/JUCERPROJECT\ id="......"/JUCERPROJECT\ id="dhaha"/g' NewProject.jucer

if [ ! -z "$JUCEDIR" ]; then
  echo "Updating JUCE directory in c_cpp_properties.json ..."
  sed -i "" "s#~/JUCE#$JUCEDIR#g" .vscode/c_cpp_properties.json
fi

echo "Replacing ALL occurrences of NewProject with $PROJECTNAME ..."
sed -i "" "s/NewProject/$PROJECTNAME/g" NewProject.jucer
find .vscode -type f -print0 | xargs -0 sed -i "" "s/NewProject/$PROJECTNAME/g"
find Source -type f -print0 | xargs -0 sed -i "" "s/NewProject/$PROJECTNAME/g"

echo "Renaming NewProject.jucer to $PROJECTNAME.jucer ..."
mv NewProject.jucer $PROJECTNAME.jucer

echo "----------------------------------------------------------------"

echo "All done! If you entered any incorrect values, please roll back to the previous commit and try again."