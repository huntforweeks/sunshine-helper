//
//  ContentView.swift
//  sunshine-helper
//
//  Created by Hunter Weeks on 11/20/21.
//

import SwiftUI

struct ContentView: View {
//    @Environment(\.managedObjectContext) private var viewContext
    
    @ObservedObject private var viewModel = ContentViewModel()
    @State public var body_exposed_percentage = UserDefaults.standard.object(forKey: "body_exposed_percentage") != nil ? UserDefaults.standard.double(forKey: "body_exposed_percentage") : 26.0
    @State public var toggle_editing = true
    
    var body: some View {
        VStack {
            HStack{
                Toggle ("Edit Parameters", isOn: $toggle_editing)
                    .toggleStyle(customButtonToggleStyle())
                    .padding()
                if self.viewModel.loading {
                    ProgressView()
                    .progressViewStyle(CircularProgressViewStyle())
                }
                else {
                    Button(action: self.get_exposure_button_handler) {
                        Text("Get exposure time")
                    }
                    .disabled(self.viewModel.loading)
                    .padding()
                    .toggleStyle(customButtonToggleStyle())
                }
            }
                
            if self.viewModel.loaded && !toggle_editing{
                VStack{
                Text("Time needed (hh:mm):\n" + viewModel.vitdtime + "\n" + viewModel.sunburntime)
                }
            }
            if toggle_editing {
                ScrollView {
    //                Text ("Parameters for sun exposure calculations.")
            //            Text ("Time of Sun Exposure (Note, times falling across UTC time boundaries seem to fail)")
            //            Picker("Time of Sun Exposure", selection: $viewModel.selected_time_of_day) {
            //                Text("Noon Today").tag(e_time_of_day_mode.e_time_of_day_around_noon)
            //                Text("Now").tag(e_time_of_day_mode.e_time_of_day_now)
            //                Text("Other Time & Date").tag(e_time_of_day_mode.e_time_of_day_custom_time)
            //            }
            //            .padding()
            //            .pickerStyle(SegmentedPickerStyle())
            //            if viewModel.selected_time_of_day == e_time_of_day_mode.e_time_of_day_custom_time {
            //                DatePicker("Custom Date", selection: $viewModel.chosen_date)
            //                    .padding()
            //                    .disabled(viewModel.selected_time_of_day != e_time_of_day_mode.e_time_of_day_custom_time)
            //            }
                    Text("Cloud Conditions")
                    Picker("", selection: $viewModel.selected_sky_condition) {
                        Text("Cloudless").tag(e_sky_condition.e_sky_cloudless)
            //                Text("Scattered").tag(e_sky_condition.e_sky_scattered)
            //                Text("Broken").tag(e_sky_condition.e_sky_broken)
                        Text("Overcast").tag(e_sky_condition.e_sky_overcast)
                    }
                    .pickerStyle(SegmentedPickerStyle())
                    .padding()
                    
                    Divider()
                    
                    let skinpercent = String(format: "%.1f", body_exposed_percentage)
                    Text("\(skinpercent)% of skin exposed, based on selections below")
                    BodyPartPicker (body_exposed_percentage: $body_exposed_percentage)
                    
                    Divider()
                    Text ("Skin Type (See: https://en.wikipedia.org/wiki/Fitzpatrick_scale )")
                    Picker("", selection: $viewModel.selected_skin_type) {
                        Text("1 ✋🏻").tag(1)
                        Text("2 ✋🏻").tag(2)
                        Text("3 ✋🏼").tag(3)
                        Text("4 ✋🏽").tag(4)
                        Text("5 ✋🏾").tag(5)
                        Text("6 ✋🏿").tag(6)
                    }
                    .padding()
                    .pickerStyle(SegmentedPickerStyle())
                    
                    Divider()
            //            Slider(value: $body_exposed_percentage, in: 0...100, step: 5)
            //            .padding()
                    Text("Data from: https://fastrt.nilu.no/README_VitD_quartMEDandMED_v2.html Note on parameters above: Time of day is hardcoded to solar noon based on your rounded gps coordinates, surface type/albedo is hardcoded to sand.")
                        .padding()
                }
                }
            Spacer()
            
        }
        
    }
    
    func get_exposure_button_handler()
    {
        self.toggle_editing = false
        self.viewModel.skin_exposed_percent = self.body_exposed_percentage
        
        self.viewModel.getExposure()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
