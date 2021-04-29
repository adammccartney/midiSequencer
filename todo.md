## Todo

30. April 2021

+ ~~add help menu (work with h key to toggle display of available modes)~~
+ ~~fix missing rvalue and prob inclusion~~
+ ~~remove note offs from osc message~~

29. April 2021
+ ~~set up Osc routing~~
+ ~~design Osc message, call is going to happen at the end of processMidiNote(), 
  as we need to access the Note queue that is a member of that object~~
+ ~~fire a tracer bullet and check that you haven't just built a pile of pooh~~

28. April 2021
+ ~~get gui to talk to logic structure for segments (selecting harmonies)~~
+ ~~make QuantizedPitchManager for each hfield and hfieldgraph instance in top
  level ofApp.h~~
+ ~~implement QuantizedPitchManager::processMidiNote()~~

27. April 2021
+ ~~add two text input fields per HarmonicField on the GUI in order to manager
  root and mode input~~
+ ~~figure out correct scope to make PitchSetManager from gui. PitchSetManager
  depends on two input criteria: root, mode~~
+ ~~open midi port for noteins~~
+ ~~fix segfault after integrating unit tests~~
   - ~~eliminate any leftover calls to mock functions in HarmonicFieldGraph~~
+ ~~integrate unit tests~~

23. April 2021
+ ~~implement and test getters for Note relative time and probabilty members~~

22. April 2021
+ ~~Figure out what's causing the segfault in the call to
  QuantizedPitchManager::processMidiNote(const int &midVal)~~

20. April 2021
+ ~~finish design of HarmonicFieldManager and QuantizedPitchManager,
  implement~~
+ ~~figure out relationships needed to connect up~~
  - ~~HarmonicField | HarmonicFieldManager QuantizedPitchManager | OscMessage~~

19. April 2021
+ ~~reformat unit tests to use Google Tests~~ (turns out a giant corporation
  does at least one thing right)

14. April 2021
+ ~~Design + implement behavior model for sender~~
+ ~~Design behavior model for receiver~~

9. April 2021
+ ~~Create enum pitchclass~~
+ ~~Ammend construction of harmonic field graphs so they don't overlap~~
+ ~~Convert python code into HarmonicField logic class~~

8. April 2021
+ ~~Create a timeline and section it according to the number of harmonic
  fields~~
