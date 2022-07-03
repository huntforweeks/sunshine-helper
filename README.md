 sunshine-helper
 ===
A simple to use calculator for answering the questions: "How much sun do I need to get my d3 for the day?" and "How much sun can I get before I sunburn"

The modern computer-age lifestyle of the western world, especially with COVID measures, can easily prevent us from getting the sun exposure that our bodies need to function properly.

# Progress/Feature Roadmap
### Done:
* Create wrapping API to call CGI of the web tool
* Pull basic data from device that shouldn’t require user input (time/date/location)
* Add UI components for core functionality
  * Fitzpatrick scale skin type
  * Body parts/areas exposed to sun picker
  * Sky Condition Picker
* Adjust layout/components to allow decent UX on desktop/phone/watch
### In progress:
* Currently in contact with the NILU researchers that own the FastRT project, and they are making their code available once again via GPL. Thus I'll soon be able to port their simulation to run locally on-device.
### Todo:
* Option for Daily notification reminder with amount of sun needed that day
* Add UI Components for manual cloud condition data entry
* Integrate against open weather API
* Add option to prefill sky conditions from weather API
* port simulation to native code for local computation
* Create UV exposure over the course of the day visualization / exposure start time picker
* Stopwatch for start/stop/pause exposure, and providing notifications during a session
* integrate with watchOS sensors to automatically detect the start of a session, and notifyuser to confirm.
