// JMGK (c) 2021

$fn = 25;

module clip() {
  linear_extrude(height = 3) {
    minkowski() {
      polygon(points = [
        [ 0, 0 ],
        [ 0, 30 ],
        [ 5, 30 ],
        [ 5, 7 ],
        [ 3, 7 ],
        [ 3, 15 ],
        [ 4, 15 ],
        [ 4, 27 ],
        [ 1, 27 ],
        [ 1, 0 ],
      ]);
      circle(0.5);
    }
  }
}

//clips
translate([ 0.5, 0, 0 ]) clip();
translate([ 0.5, 0, 7 ]) clip();

//base
cube([ 2, 12, 10 ]);

//pino
translate([ -2, 5, 2 ]) rotate([ 0, 90, 0 ]) cylinder(4, 1.4, 1.4);

//tabs
translate([ -1, 0, 9.9 ]) cube([ 3, 1, 1 ]);
translate([ -1, 5, 9.9 ]) cube([ 3, 1, 1 ]);