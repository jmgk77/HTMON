// JMGK (c) 2021

include <MCAD/boxes.scad>
include <OpenSCAD_fasteners/nuts_and_bolts.scad>

// vars

box_largura = 40;
box_comprimento = 60;
box_f_altura = 5;
box_b_altura = 10;

delta_furacao = 5;

wemos_largura = 25.6;
wemos_comprimento = 34.2;
wemos_altura = 3.5;
wemos_furacao = 3;
wemos_hole = 0.8;

oled_largura = 26.3;
oled_comprimento = 26.3;
oled_altura1 = 1.2; // altura placa
oled_altura2 = 1.1; // altura fiação interna
oled_altura3 = 1.2; // altura visor
oled_altura = oled_altura1 + oled_altura2 + oled_altura3;
oled_hole = 0.8;

oled_delta = 4;
oled_furacao = 2;

//

tolerance = 0.40;
tolerance2 = 0.01;

$fn = 25;

//

module front_plate() {
  difference() {
    // caixa
    roundedCube([ box_largura, box_comprimento, box_f_altura ], 3, true, false);

    // visor
    translate([ 7, 25, 0 ]) scale([ 1 + tolerance2, 1 + tolerance2, 1 ])
        oled_96();

    // porcas
    translate([ delta_furacao, delta_furacao, 0 ]) nutHole(metric_fastener[3]);
    translate([ delta_furacao, box_comprimento - delta_furacao, 0 ])
        nutHole(metric_fastener[3]);
    translate([ box_largura - delta_furacao, delta_furacao, 0 ])
        nutHole(metric_fastener[3]);
    translate(
        [ box_largura - delta_furacao, box_comprimento - delta_furacao, 0 ])
        nutHole(metric_fastener[3]);
  }
}

module back_plate() {
  difference() {
    // caixa
    roundedCube([ box_largura, box_comprimento, box_b_altura ], 3, true, false);

    // wemos
    translate([
      (box_largura - (wemos_largura + (2 * tolerance))) / 2,
      (box_comprimento - (wemos_comprimento + (2 * tolerance))) / 2,
      box_b_altura -
      wemos_altura
    ]) scale([ 1 + tolerance2, 1 + tolerance2, 1 ]) wemos();

    // fiação wemos
    translate([
      (box_largura - (wemos_largura + (2 * tolerance))) / 2,
      (box_comprimento - (wemos_comprimento + (2 * tolerance))) / 2,
      box_b_altura - (wemos_altura + 5)
    ]) wemos_wiring();

    // wemos reset hole
    translate([
      0, ((box_comprimento - (wemos_comprimento - (2 * tolerance))) / 2) + 3,
      (box_b_altura - wemos_altura) + 1
    ]) rotate(90, [ 0, 1, 0 ]) cylinder(box_b_altura, 1, 1);

    // fiação visor
    translate([
      (box_largura - 12) / 2,
      wemos_comprimento + ((box_comprimento - wemos_comprimento) / 2) -
          tolerance,
      2
    ]) cube([ 12, 5, 8 ]);

    // furação
    translate([ delta_furacao, delta_furacao, 0 ]) {
      boltHole(metric_fastener[3]);
      cylinder(3.5, 3, 3);
    }
    translate([ delta_furacao, box_comprimento - delta_furacao, 0 ]) {
      boltHole(metric_fastener[3]);
      cylinder(3.5, 3, 3);
    }
    translate([ box_largura - delta_furacao, delta_furacao, 0 ]) {
      boltHole(metric_fastener[3]);
      cylinder(3.5, 3, 3);
    }
    translate(
        [ box_largura - delta_furacao, box_comprimento - delta_furacao, 0 ]) {
      boltHole(metric_fastener[3]);
      cylinder(3.5, 3, 3);
    }
  }
}

module oled_96() {
  difference() {
    // visor
    union() {
      cube([
        oled_largura, oled_comprimento,
        box_f_altura - (oled_altura2 + oled_altura3)
      ]);
      translate([ oled_delta, 0, box_f_altura - (oled_altura2 + oled_altura3) ])
          cube([
            oled_largura - (oled_delta * 2), oled_comprimento,
            oled_altura2
          ]);
      translate([ 0, oled_delta, box_f_altura - (oled_altura2 + oled_altura3) ])
          cube([
            oled_largura, oled_comprimento - (oled_delta * 2), oled_altura2 +
            oled_altura3
          ]);
    }
    // furos
    translate([ oled_furacao, oled_furacao, 0 ])
        cylinder(5, oled_hole, oled_hole);
    translate([ oled_largura - oled_furacao, oled_furacao, 0 ])
        cylinder(5, oled_hole, oled_hole);
    translate([ oled_furacao, oled_comprimento - oled_furacao, 0 ])
        cylinder(5, oled_hole, oled_hole);
    translate(
        [ oled_largura - oled_furacao, oled_comprimento - oled_furacao, 0 ])
        cylinder(5, oled_hole, oled_hole);
  }
}

module wemos() {
  difference() {
    cube([ wemos_largura, wemos_comprimento, wemos_altura ]);
    // furos
    translate([ wemos_largura - wemos_furacao, wemos_furacao, 0 ])
        cylinder(wemos_altura, wemos_hole, wemos_hole);
    translate([ wemos_furacao, wemos_comprimento - wemos_furacao, 0 ])
        cylinder(wemos_altura, wemos_hole, wemos_hole);
  }
}

module wemos_wiring() {
  difference() {
    cube([ wemos_largura, wemos_comprimento, 5 ]);
    // bases
    translate([ wemos_largura - wemos_furacao, wemos_furacao, 0 ])
        cylinder(5, 2, 2);
    translate([ wemos_furacao, wemos_comprimento - wemos_furacao, 0 ])
        cylinder(5, 2, 2);
  }
  // furo inferior para fiação
  translate([ wemos_largura / 2, 0, 2.5 ]) rotate(90, [ 1, 0, 0 ])
      cylinder(15, 3, 3);
}

// main
rotate(180, [ 0, 1, 0 ]) translate([ 5, 0, -5 ]) front_plate();
back_plate();
