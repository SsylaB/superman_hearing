import("stdfaust.lib");

// Freq from pot (200-2000 Hz)
f = hslider("freq", 440, 200, 2000, 0.01) : si.smoo;

// Gate from button (toggle on/off)
gate = button("gate");

// Sine osc * gain, gated
process = os.osc(f) * (gate * 0.2) <: _,_;

