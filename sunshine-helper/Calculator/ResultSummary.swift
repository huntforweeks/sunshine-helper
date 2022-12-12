//
//  ResultSummary.swift
//  sunshine-helper
//
//  Created by Hunter Weeks on 12/11/22.
//

import SwiftUI

struct OrangeRedBorderGradient: View{
    var body: some View {
        Rectangle()
            .stroke(
                LinearGradient(
                    gradient: Gradient(colors: [Color.red, Color.yellow]),
                    startPoint: .leading,
                    endPoint: .trailing
                ),
                lineWidth: 5
            )
    }
}

struct GreenBlueBorderGradient: View{
    var body: some View {
        Rectangle()
            .stroke(
                LinearGradient(
                    gradient: Gradient(colors: [Color.green, Color.blue]),
                    startPoint: .leading,
                    endPoint: .trailing
                ),
                lineWidth: 5
            )
    }
}


struct ResultSummary: View {
    @EnvironmentObject private var viewModel: ContentViewModel
    var body: some View {
        VStack {
            HStack {
                VStack {
                    if (viewModel.exposure_result.erythemaPercentDose < 1.0)
                    {
                        Text(viewModel.sunburntimeshort)
                            .font(.largeTitle)
                            .padding()
                            .shadow(radius: 5.0)
                            .background(LinearGradient(gradient: Gradient(colors: [Color.red.opacity(0.3), Color.yellow.opacity(0.3)]), startPoint: .leading, endPoint: .trailing))
                            .cornerRadius(10)
                            .shadow(radius: 5.0)
                    }
                    else
                    {
                        Text(viewModel.sunburntimeshort)
                            .font(.largeTitle)
                            .padding()
                            .background(LinearGradient(gradient: Gradient(colors: [Color.red, Color.yellow]), startPoint: .leading, endPoint: .trailing))
                            .cornerRadius(10)
                            .shadow(radius: 5.0)
                    }
                    if (viewModel.exposure_result.vitdPercentDose < 1.0)
                    {
                        Text(viewModel.vitdtimeshort)
                            .font(.largeTitle)
                            .padding()
                            .background(LinearGradient(gradient: Gradient(colors: [Color.blue.opacity(0.3), Color.green.opacity(0.3)]), startPoint: .leading, endPoint: .trailing))
                            .shadow(radius: 5.0)
                    }
                    else
                    {
                        Text(viewModel.vitdtimeshort)
                            .font(.largeTitle)
                            .foregroundColor(.white)
                            .padding()
                            .background(LinearGradient(gradient: Gradient(colors: [Color.blue, Color.green]), startPoint: .leading, endPoint: .trailing))
                            .cornerRadius(10)
                            .shadow(radius: 5.0)
                    }
                }
                VStack {
                    Text(viewModel.sunburntime)
                        .font(.title2)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding()
                    Text(viewModel.vitdtime)
                        .font(.title2)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding()
                }
            }
            .padding()
            Text(viewModel.formattedResultSummary)
                .font(.title3)
                .frame(maxWidth: .infinity, alignment: .leading)
                .padding()
        }
    }
}

struct ResultSummary_Previews: PreviewProvider {
    static var previews: some View {
        ResultSummary()
    }
}
