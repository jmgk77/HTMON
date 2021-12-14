// JMGK (c) 2021

include <MCAD/boxes.scad>
include <OpenSCAD_fasteners/nuts_and_bolts.scad>

use <ruler.scad>
// vars

base_altura = 2;
case_altura = 30;
box_largura = 110;
box_comprimento = 60;
case_wall = 1.2;

delta_furacao = 4;

isolador_delta = 8.3;
isolador6_x = 15;
isolador6_y = (box_comprimento - (4 * isolador_delta)) / 2;
isolador3_x = box_largura - 15;
isolador3_y = (box_comprimento - (1 * isolador_delta)) / 2;

rele_largura = 26.5;
rele_comprimento = 34;
rele_x = 25;
rele_y = (box_comprimento - (rele_comprimento + (2 * 2))) / 2;
rele_delta = 2.5;

pwr_largura = 21.3;
pwr_comprimento = 35.3;
pwr_wall = 0.8;
pwr_x = 60;
pwr_y = (box_comprimento - (35.3 + (2 * pwr_wall))) / 2;
//

tolerance = 0.40;
tolerance2 = 0.01;

$fn = 25;

//

module case () {
  difference() {
    cube([ box_largura, box_comprimento, case_altura ]);
    translate([ case_wall, case_wall, case_wall ]) cube([
      box_largura - (2 * case_wall), box_comprimento - (2 * case_wall),
      case_altura
    ]);

    // furos fios
    translate([ -5, box_comprimento / 3, case_altura / 3 ])
        rotate(90, [ 0, 1, 0 ]) cylinder(box_largura + 10, 3, 3.5);
    translate([ -5, (box_comprimento / 3) * 2, case_altura / 3 ])
        rotate(90, [ 0, 1, 0 ]) cylinder(box_largura + 10, 3, 3.5);
  }

  difference() {
    union() { // encaixe
      translate([ delta_furacao, delta_furacao, 0 ]) {
        cylinder(case_altura - (5 - base_altura) - tolerance, 3, 3);
      }
      translate(
          [ box_largura - delta_furacao, box_comprimento - delta_furacao, 0 ]) {
        cylinder(case_altura - (5 - base_altura) - tolerance, 3, 3);
      }
      translate([ delta_furacao, box_comprimento - delta_furacao, 0 ]) {
        cylinder(case_altura - (5 - base_altura) - tolerance, 3, 3);
      }
      translate([ box_largura - delta_furacao, delta_furacao, 0 ]) {
        cylinder(case_altura - (5 - base_altura) - tolerance, 3, 3);
      }
    }

    // furos encaixe
    translate([ delta_furacao, delta_furacao, 17 ]) {
      boltHole(metric_fastener[3]);
    }
    translate(
        [ box_largura - delta_furacao, box_comprimento - delta_furacao, 17 ]) {
      boltHole(metric_fastener[3]);
    }
    translate([ delta_furacao, box_comprimento - delta_furacao, 17 ]) {
      boltHole(metric_fastener[3]);
    }
    translate([ box_largura - delta_furacao, delta_furacao, 17 ]) {
      boltHole(metric_fastener[3]);
    }
  }
}

module back_plate() {
  difference() {
    union() {
      cube([ box_largura, box_comprimento, base_altura ]);

      // encaixe
      translate([ delta_furacao, delta_furacao, 0 ]) { cylinder(5, 3, 3); }
      translate(
          [ box_largura - delta_furacao, box_comprimento - delta_furacao, 0 ]) {
        cylinder(5, 3, 3);
      }
      translate([ delta_furacao, box_comprimento - delta_furacao, 0 ]) {
        cylinder(5, 3, 3);
      }
      translate([ box_largura - delta_furacao, delta_furacao, 0 ]) {
        cylinder(5, 3, 3);
      }

      // pinos isoladores
      for (i = [0:4]) {
        translate([ isolador6_x, isolador6_y + (i * isolador_delta), 0 ]) {
          cylinder(12, 1, 1);
        }
      }
      for (i = [0:1]) {
        translate([ isolador3_x, isolador3_y + (i * isolador_delta), 0 ]) {
          cylinder(12, 1, 1);
        }
      }

      // base relé 34x26.5
      translate([ rele_x + rele_delta, rele_y + rele_delta, 0 ]) {
        cylinder(5, 3, 3);
        cylinder(10, 1.3, 1.3);
      }
      translate(
          [ rele_x + rele_delta, rele_y + rele_comprimento - rele_delta, 0 ]) {
        cylinder(5, 3, 3);
        cylinder(10, 1.3, 1.3);
      }
      translate(
          [ rele_x + rele_largura - rele_delta, rele_y + rele_delta, 0 ]) {
        cylinder(5, 3, 3);
        cylinder(10, 1.3, 1.3);
      }
      translate([
        rele_x + rele_largura - rele_delta,
        rele_y + rele_comprimento - rele_delta, 0
      ]) {
        cylinder(5, 3, 3);
        cylinder(10, 1.3, 1.3);
      }

      // power 21.3x35.3
      translate([ pwr_x, pwr_y, 0 ]) cube([
        pwr_largura + (2 * pwr_wall), pwr_comprimento + (2 * pwr_wall), 10
      ]);
    }

    // furos encaixe
    translate([ delta_furacao, delta_furacao, 0 ]) {
      boltHole(metric_fastener[3]);
    }
    translate(
        [ box_largura - delta_furacao, box_comprimento - delta_furacao, 0 ]) {
      boltHole(metric_fastener[3]);
    }
    translate([ delta_furacao, box_comprimento - delta_furacao, 0 ]) {
      boltHole(metric_fastener[3]);
    }
    translate([ box_largura - delta_furacao, delta_furacao, 0 ]) {
      boltHole(metric_fastener[3]);
    }

    // power 21.3x35.3
    translate([ pwr_x + pwr_wall, pwr_y + pwr_wall, 2 ])
        cube([ pwr_largura, pwr_comprimento, 8 ]);
  }
}

module washers() {
  for (i = [1:4])
    translate([ i * 12, -10, 0 ]) difference() {
      cylinder(2, 5, 5);
      boltHole(metric_fastener[3]);
    }
}

translate([ 0 - (box_largura + 10), 0, 0 ]) back_plate();
case(); 
washers();
