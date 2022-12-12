//
//  sunshine_helperApp.swift
//  sunshine-helper
//
//  Created by Hunter Weeks on 11/20/21.
//

import SwiftUI

@main
struct sunshine_helperApp: App {
    
    @StateObject var sunshine_shared_api = ContentViewModel()
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(sunshine_shared_api)
//                .environment(\.managedObjectContext, persistenceController.container.viewContext)

        }
    
    }
}
