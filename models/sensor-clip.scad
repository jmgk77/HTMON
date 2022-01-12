//HTMON
//Copyright JMGK 2021-2022

//Sensor clip OpenSCAD model

//This file is part of HTMON
//This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.

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
translate([ -3, 0, 9.9 ]) cube([ 5, 3, 1 ]);
translate([ -3, 5, 9.9 ]) cube([ 5, 3, 1 ]);