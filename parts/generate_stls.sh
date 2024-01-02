#!/bin/bash

# enter directory of script (parts)
cd "$(dirname "$0")"

# Detect OS
if [[ "$OSTYPE" == "darwin"* ]]; then
  echo "Mac OS X detected"
  SCAD="open -n -a OpenSCAD --args"
else
  echo "Linux detected"
  SCAD="openscad"
fi

echo "Deleting previous build output"
rm -rf stl
mkdir -p stl/optional

for MODULE in agitation enclosure frame lower_rail upper_rail
do
  PARTS=$(grep -o "part.*//.*\[.*]" ${MODULE}.scad | sed 's/,/ /g' | sed 's/.*\[\([^]]*\)\].*/\1/g')
  echo "generating from ${MODULE}:"
  for PART in ${PARTS}
  do
    echo ${PART}
    if [[ "${PART}" != "OPTIONAL"* ]]; then
      FILENAME=$(echo stl/${MODULE}_${PART}.stl | tr '[:upper:]' '[:lower:]')
      $SCAD $(pwd)/${MODULE}.scad --D part=\"${PART}\" --o $(pwd)/${FILENAME}
    fi
  done
done

#hack for parts with modifyers
$SCAD $(pwd)/lower_rail.scad --D part=\"Interface\" --D light_trap=\"true\" --o $(pwd)/stl/lower_rail_interface_light_trap.stl
$SCAD $(pwd)/lower_rail.scad --D part=\"Magnetic_holder\" --D rod_mount=false --o $(pwd)/stl/lower_rail_magnetic_holder_wg.stl

#hack for optional instruments
$SCAD $(pwd)/lower_rail.scad --D part=\"OPTIONAL_tapping_tool\" --o $(pwd)/stl/optional/tapping_tool_top_vessel.stl
$SCAD $(pwd)/lower_rail.scad --D part=\"OPTIONAL_tapping_tool\" --D onside=true --o $(pwd)/stl/optional/tapping_tool_side_vessel.stl
$SCAD $(pwd)/lower_rail.scad --D part=\"OPTIONAL_tapping_tool\" --D light_trap=true --o $(pwd)/stl/optional/tapping_tool_top_tank.stl
$SCAD $(pwd)/lower_rail.scad --D part=\"OPTIONAL_tapping_tool\" --D light_trap=true --D onside=true --o $(pwd)/stl/optional/tapping_tool_side_tank.stl
$SCAD $(pwd)/agitation.scad --D part=\"OPTIONAL_servo_gauge\" --o $(pwd)/stl/optional/servo_gauge.stl
$SCAD $(pwd)/frame.scad --D part=\"OPTIONAL_nut_spinner\" --o $(pwd)/stl/optional/nut_spinner.stl
$SCAD $(pwd)/lower_rail.scad --D part=\"OPTIONAL_threading_tool\" --o $(pwd)/stl/optional/threading_tool.stl
$SCAD $(pwd)/lower_rail.scad --D part=\"OPTIONAL_wrench\" --o $(pwd)/stl/optional/wrench.stl
