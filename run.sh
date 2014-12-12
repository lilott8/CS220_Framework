#/bin/sh

#List of all arguments
ALGORITHMS=(lee lee2bit lee3bit ruben hadlock)
DIRECTION=( 0 1 )
INTERSECTION=( 0 1 )
KORN=( 1.0 1.5)

#File from input
DIRECTORY=$1

SPACE=" "
TEST_FILES="${DIRECTORY}Tests/*.json"

for f in $TEST_FILES
do
  for a in "${ALGORITHMS[@]}"
  do
    echo "******************"
    echo "Starting "$a
    echo "******************"
    for d in "${DIRECTION[@]}"
    do
      for i in "${INTERSECTION[@]}"
      do
        if [ "$a" = "ruben" ]
        then
          for k in "${KORN[@]}"
          do
            COMMAND="${DIRECTORY}grid_router ${f}${SPACE}${a}${SPACE}${d}${SPACE}${i}${SPACE}${k}"
            echo ${COMMAND}
            ${COMMAND}
          done
        else
          COMMAND="${DIRECTORY}grid_router ${f}${SPACE}${a}${SPACE}${d}${SPACE}${i}"
          echo ${COMMAND}
          ${COMMAND}
        fi
      done
    done
    echo ""
  done
done