//
//  ContentView.swift
//  sunshine-helper
//
//  Created by Hunter Weeks on 11/20/21.
//

import SwiftUI
import SwiftUICharts

struct ContentView: View {
//    @Environment(\.managedObjectContext) private var viewContext
    
    @EnvironmentObject private var viewModel: ContentViewModel
    @State public var body_exposed_percentage = UserDefaults.standard.object(forKey: "body_exposed_percentage") != nil ? UserDefaults.standard.double(forKey: "body_exposed_percentage") : 26.0
    @State public var toggle_editing = true
    
    var body: some View {
        TabView {
            ParametersEditView()
                .tabItem {
                    Label("Calculator", systemImage: "square.and.pencil")
                }
//                .onAppear {
//                    if #available(iOS 15, *) {
//                        let appearance = UINavigationBarAppearance()
//                        appearance.configureWithOpaqueBackground()
//                        UINavigationBar.appearance().standardAppearance = appearance
//                        UINavigationBar.appearance().scrollEdgeAppearance = appearance
//                    }
//                    if #available(iOS 16, *) {
//                        let appearance = UINavigationBarAppearance()
//                        appearance.configureWithOpaqueBackground()
//                        UINavigationBar.appearance().standardAppearance = appearance
//                        UINavigationBar.appearance().scrollEdgeAppearance = appearance
//                    }
//                }

//            StopwatchView()
//                .tabItem {
//                    Label("Stopwatch", systemImage: "stopwatch")
//                }
        }
    }
    

}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        Group {
            ContentView()
        }
    }
}
