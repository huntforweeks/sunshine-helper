//
//  ContentView.swift
//  sunshine-helper WatchKit Extension
//
//  Created by Hunter Weeks on 11/20/21.
//

import SwiftUI
import SwiftUICharts

struct ContentView: View {
    
    @ObservedObject private var viewModel = ContentViewModel()
    @State public var body_exposed_percentage = 26.0
    
    @State public var toggle_editing = true
    
    @State private var isShowingBodyPartPicker = false
    
    var body: some View {
        List {

                NavigationLink("Results", destination: List {
                    if self.viewModel.loading {
                        ProgressView()
                        .progressViewStyle(CircularProgressViewStyle())
                    }
                    else {
                        Button(action: get_exposure_button_handler) {
                            Text("Get exposure time")
                        }
                        .disabled(self.viewModel.loading)
                        .padding()
                    }
                    if self.viewModel.loaded {
                    Text("Time needed (hh:mm):\n" + viewModel.vitdtime + "\n" + viewModel.sunburntime)
                        LineChartView(dataPoints: viewModel.sunAngleData.map({$0.getDataPoint()})
                            .filter({$0.endValue > 0.0})
                        )
                        .chartStyle(
                             LineChartStyle(
                                 lineMinHeight: 50,
                                 showAxis: false,
                                 axisLeadingPadding: 0,
                                 showLabels: true,
                                 labelCount: 5,
                                 showLegends: true,
                                 drawing: .fill
                             )
                         )
                    }
                })

                NavigationLink("Parameters", destination: List {
    //                Toggle ("Edit Parameters", isOn: $toggle_editing)
                    Picker("Time of Sun Exposure", selection: $viewModel.selected_time_of_day) {
                        Text("Noon Today").tag(e_time_of_day_mode.e_time_of_day_around_noon)
                        Text("Now").tag(e_time_of_day_mode.e_time_of_day_now)
                    }
                     Picker("Cloud Conditions", selection: $viewModel.selected_sky_condition) {
                        Text("Cloudless").tag(e_sky_condition.e_sky_cloudless)
                            Text("Scattered").tag(e_sky_condition.e_sky_scattered)
                            Text("Broken").tag(e_sky_condition.e_sky_broken)
                        Text("Overcast").tag(e_sky_condition.e_sky_overcast)
                    }
                    

                    let skinpercent = String(format: "%.1f", body_exposed_percentage)

                    NavigationLink("\(skinpercent)% of skin exposed. Click to change", destination: List {
                    Text("\(skinpercent)% of skin exposed, based on selections below")
                        BodyPartPicker (body_exposed_percentage: $body_exposed_percentage)
                    }
                    )
                    Picker("Skin Type (Based on the Fitzpatrick scale)", selection: $viewModel.selected_skin_type) {
                        Text("Type 1 ✋🏻").tag(1)
                        Text("Type 2 ✋🏻").tag(2)
                        Text("Type 3 ✋🏼").tag(3)
                        Text("Type 4 ✋🏽").tag(4)
                        Text("Type 5 ✋🏾").tag(5)
                        Text("Type 6 ✋🏿").tag(6)
                    }
                    .padding()
                    
                }
                )
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
