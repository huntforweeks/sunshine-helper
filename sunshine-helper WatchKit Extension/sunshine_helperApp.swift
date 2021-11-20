//
//  sunshine_helperApp.swift
//  sunshine-helper WatchKit Extension
//
//  Created by Hunter Weeks on 11/20/21.
//

import SwiftUI

@main
struct sunshine_helperApp: App {
    @SceneBuilder var body: some Scene {
        WindowGroup {
            NavigationView {
                ContentView()
            }
        }

        WKNotificationScene(controller: NotificationController.self, category: "myCategory")
    }
}
