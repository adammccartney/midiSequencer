/* Quantizer.h: sets up a simple pitch quantizer 
 *
 * input: midi note in the range 0-127
 * process: use floor function to find nearest midi note in
 * *PitchManager->pitchClassSet, 
 * then transposes pitch -12 in order to find relevant octave 
 * *PitchManager->pitchSet
 * output: midi note in the range 0-127
 *
 * /
