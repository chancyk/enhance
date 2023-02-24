import std/math
import std/tables
import std/times, std/monotimes
import std/json
import std/streams
# import jsony


type Entry = object
  x0: float
  y0: float
  x1: float
  y1: float


# calculate haversin
proc haversine_of_degrees(x0, y0, x1, y1, R: float): float =
  let
    dY = degToRad(y1 - y0)
    dX = degToRad(x1 - x0)
    r_y0 = degToRad(y0)
    r_y1 = degToRad(y1)

  let root_term = (sin(dY/2)^2) + cos(r_y0) * cos(r_y1) * (sin(dX/2)^2)
  result = 2 * R * arcsin(sqrt(root_term))


let sample = """{
  "pairs": [
    {"x0": 1.1, "y0": 2.2, "x1": 3.3, "y1": 4.4},
    {"x0": 1.2, "y0": 2.3, "x1": 3.4, "y1": 4.5}
  ]
}"""

# Read in a file
let t0 = getMonotime()
# let data = readFile("data_10000000_flex.json")
let s = newFileStream("data_10000000_flex.json", fmRead)

let t1 = getMonotime()
let parsed = parseJson(s)
# let parsed = data.fromJson(Table[string, seq[Entry]])
let t2 = getMonotime()

let earth_radius_km = 6371
var total: float = 0
var count: int = 0
for pair in parsed["pairs"]:
    total += haversine_of_degrees(pair["x0"].fnum, pair["y0"].fnum, pair["x1"].fnum, pair["y1"].fnum, earth_radius_km.float)
    count += 1
let average = total / count.float

let t3 = getMonotime()

echo "Result: ", average
echo "Input: ", (t1 - t0).inMilliseconds.float64 / 1000
echo "Parse: ", (t2 - t1).inMilliseconds.float64 / 1000
echo "Calc: ", (t3 - t2).inMilliseconds.float64 / 1000
echo "Total: ", (t3 - t0).inMilliseconds.float64 / 1000
