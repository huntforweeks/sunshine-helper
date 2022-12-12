//
//  ParametersEditView.swift
//  sunshine-helper
//
//  Created by Hunter Weeks on 12/11/22.
//

import SwiftUI

struct ParametersEditView: View {
    @EnvironmentObject private var viewModel: ContentViewModel
    @State public var body_exposed_percentage = UserDefaults.standard.object(forKey: "body_exposed_percentage") != nil ? UserDefaults.standard.double(forKey: "body_exposed_percentage") : 26.0
    
    
    var body: some View {
        NavigationView {
                ScrollView {
                    NavigationLink {
                        ResultView()
                            .onAppear { self.viewModel.getExposure()}
                            .environmentObject(viewModel)
                    } label: {
                            Label("Run Sun Exposure Calculation", systemImage: "sun.haze.fill")
                            .font(.title)
                            .foregroundColor(.white)
                            .padding()
                            .background(LinearGradient(gradient: Gradient(colors: [Color.orange, Color.blue]), startPoint: .leading, endPoint: .trailing))
                            .cornerRadius(40)
                            .shadow(radius: 5.0)
                    }
                    Spacer()
                    Divider()
                    Text ("Parameters:")
                        .font(.title)
                    Group {
                        Picker("Time of Sun Exposure", selection: $viewModel.selected_time_of_day) {
                            Text("Noon Today").tag(e_time_of_day_mode.e_time_of_day_around_noon)
                            Text("Now").tag(e_time_of_day_mode.e_time_of_day_now)
                            Text("Other Time & Date").tag(e_time_of_day_mode.e_time_of_day_custom_time)
                        }
                        .padding()
                        .pickerStyle(SegmentedPickerStyle())
                        if viewModel.selected_time_of_day == e_time_of_day_mode.e_time_of_day_custom_time {
                            DatePicker("Custom Date", selection: $viewModel.chosen_date)
                                .padding()
                                .disabled(viewModel.selected_time_of_day != e_time_of_day_mode.e_time_of_day_custom_time)
                        }
                    }
                    Group {
                        Text("Cloud Conditions")
                        .font(.title2)
                        Picker("", selection: $viewModel.selected_sky_condition) {
                            Text("Cloudless").tag(e_sky_condition.e_sky_cloudless)
                            Text("Scattered").tag(e_sky_condition.e_sky_scattered)
                            Text("Broken").tag(e_sky_condition.e_sky_broken)
                            Text("Overcast").tag(e_sky_condition.e_sky_overcast)
                        }
                        .pickerStyle(SegmentedPickerStyle())
                        .padding()
                    }
                    
                    Divider()
                    Group {
                        let skinpercent = String(format: "%.1f", body_exposed_percentage)
                        Text("\(skinpercent)% of skin exposed, based on selections below")
                        .font(.title2)
                        BodyPartPicker (body_exposed_percentage: $body_exposed_percentage)
                            .onChange(of: body_exposed_percentage) { newValue in
                                self.viewModel.skin_exposed_percent = self.body_exposed_percentage
                            }
                        
                        Divider()
                        Text ("Skin Type (See: https://en.wikipedia.org/wiki/Fitzpatrick_scale )")
                        .font(.title2)
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
                    }
                    
                    Divider()
                    Text("Modified open source code from: https://fastrt.nilu.no/README_VitD_quartMEDandMED_v2.html")
                        .padding()
                }
                .navigationBarTitle("Sun Exposure Calculator for D3 and Sunburn")
                .background(LinearGradient(gradient: Gradient(colors: [Color.orange.opacity(0.04), Color.purple.opacity(0.05)]), startPoint: .top, endPoint: .bottom))

        }
        .navigationViewStyle(StackNavigationViewStyle.stack)
        
        
    }
}

struct ParametersEditView_Previews: PreviewProvider {
    static var previews: some View {
        ParametersEditView()
    }
}
