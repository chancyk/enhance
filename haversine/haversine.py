# ========================================================================
# LISTING 21
# ========================================================================

from math import radians, sin, cos, sqrt, asin
import time
import json
import orjson
from numba import njit

JSONFile = open('data_10000000_flex.json')

#
# Read the input
#

@njit
def HaversineOfDegrees(X0, Y0, X1, Y1, R):

  dY = radians(Y1 - Y0)
  dX = radians(X1 - X0)
  Y0 = radians(Y0)
  Y1 = radians(Y1)

  RootTerm = (sin(dY/2)**2) + cos(Y0)*cos(Y1)*(sin(dX/2)**2)
  Result = 2*R*asin(sqrt(RootTerm))
  return Result


def computer_haversines(json_input):
    EarthRadiuskm = 6371
    Sum = 0
    Count = 0
    for Pair in json_input['pairs']:
        Sum += HaversineOfDegrees(Pair['x0'], Pair['y0'], Pair['x1'], Pair['y1'], EarthRadiuskm)
        Count += 1
    Average = Sum / Count
    return Average, Count


StartTime = time.time()
# JSONInput = json.load(JSONFile)
JSONInput = orjson.loads(JSONFile.read())
MidTime = time.time()

#
# Average the haversines
#

Average, Count = computer_haversines(JSONInput)
EndTime = time.time()

#
# Display the result
#

print("Result: " + str(Average))
print("Input = " + str(MidTime - StartTime) + " seconds")
print("Math = " + str(EndTime - MidTime) + " seconds")
print("Total = " + str(EndTime - StartTime) + " seconds")
print("Throughput = " + str(Count/(EndTime - StartTime)) + " haversines/second")
