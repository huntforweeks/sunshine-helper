//
//  ResultView.swift
//  sunshine-helper
//
//  Created by Hunter Weeks on 12/11/22.
//

import SwiftUI
import SwiftUICharts

struct ResultView: View {
    @EnvironmentObject private var viewModel: ContentViewModel
    var body: some View {
        return ScrollView {
            ResultSummary()
                .environmentObject(viewModel)
            Spacer()
            Divider()
            if (viewModel.exposure_result.erythemaPercentDose > 0.0 ||
                viewModel.exposure_result.vitdPercentDose > 0.0)
            {
                Group {
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
                            showLegends: true
                        )
                    )
                    Spacer()
                    Group {
                        Divider()
                        Text("Data for nerds")
                        Text("Calculated Total UV Exposure from 290nm to 400nm")
                        LineChartView(dataPoints: viewModel.spectrumDoseData.map({$0.getDataPoint()}))
                            .chartStyle(
                                LineChartStyle(
                                    lineMinHeight: 50,
                                    showAxis: false,
                                    axisLeadingPadding: 0,
                                    showLabels: true,
                                    labelCount: 5,
                                    showLegends: false
                                )
                            )
                        Text("Relative Weight for Sunburn From 290nm to 400nm")
                        LineChartView(dataPoints: viewModel.erythemaActionSpectrum.map({$0.getDataPoint()}))
                            .chartStyle(
                                LineChartStyle(
                                    lineMinHeight: 50,
                                    showAxis: false,
                                    axisLeadingPadding: 0,
                                    showLabels: true,
                                    labelCount: 5,
                                    showLegends: false,
                                    drawing: .fill
                                )
                            )
                        Text("Relative Weight for Vitamin D3 Synthesis From 290nm to 400nm")
                        LineChartView(dataPoints: viewModel.vitaminDActionSpectrum.map({$0.getDataPoint()}))
                            .chartStyle(
                                LineChartStyle(
                                    lineMinHeight: 50,
                                    showAxis: false,
                                    axisLeadingPadding: 0,
                                    showLabels: true,
                                    labelCount: 5,
                                    showLegends: false,
                                    drawing: .fill
                                )
                            )
                    }
                }
            }
        }
        .navigationBarTitle("Calculation Results")
        .navigationBarTitleDisplayMode(NavigationBarItem.TitleDisplayMode.inline)
        .background(LinearGradient(gradient: Gradient(colors: [Color.blue.opacity(0.05), Color.yellow.opacity(0.05)]), startPoint: .top, endPoint: .bottom))
    }
}

struct ResultView_Previews: PreviewProvider {
    static var previews: some View {
        ResultView()
    }
}
