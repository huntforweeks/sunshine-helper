//
//  IntentHandler.swift
//  Sunshine Helper Siri Intent
//
//  Created by Hunter Weeks on 12/10/22.
//

import Intents

class IntentHandler: INExtension {
    
    override func handler(for intent: INIntent) -> Any {
        if (intent is SunshineStrengthNowIntent)
        {
            return SunshineStrengthNowHandler()
        }
        else if (intent is SunshineStrengthTodayIntent)
        {
            return SunshineStrengthTodayHandler()
        }
        else {
            fatalError("Unhandled Intent error : \(intent)")
          }
        
        
    }
}


