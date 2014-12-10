#/bin/sh

#List of all arguments
ALGORITHMS=(lee lee2bit lee3bit ruben hadlock)
DIRECTION=( 0 1 )
INTERSECTION=( 0 1 )
KORN=( 1.0 1.5)

#File from input
TEST_FILE=$1

SPACE=" "

for a in "${ALGORITHMS[@]}"
do
  echo "******************"
  echo "Starting "$a
  echo "******************"
  for d in "${DIRECTION[@]}"
  do
    for i in "${INTERSECTION[@]}"
    do
      if [ "$a" = "RUBEN" ]
      then
        for k in "${KORN[@]}"
        do
          COMMAND="./grid_router "$TEST_FILE$SPACE$a$SPACE$d$SPACE$i$SPACE$k
          echo $COMMAND
        done
      else
        COMMAND="./grid_router "$TEST_FILE$SPACE$a$SPACE$d$SPACE$i
        echo $COMMAND
      fi
    done
  done
  echo ""
done
