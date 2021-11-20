//
//  SwiftUIView.swift
//  sunshine-helper
//
//  Created by Hunter Weeks on 11/29/21.
//

import SwiftUI

struct customButtonToggleStyle: ToggleStyle {
    var color: Color = .blue
    var lightercolor: Color = .secondary
    func makeBody(configuration: Configuration) -> some View {
        configuration.label
            .foregroundColor(configuration.isOn ? .white : color)
                .padding()
                .background(RoundedRectangle(cornerRadius: 5).fill(color).opacity(configuration.isOn ? 1.0 : 0.0))
                .onTapGesture {
                    withAnimation {
                        configuration.$isOn.wrappedValue.toggle()
                    }
                }
    }
}

struct BodyPartPicker: View {
    @Binding public var body_exposed_percentage: Double
    
    @State public var toggle_face = UserDefaults.standard.object(forKey: "toggle-body-part-face") != nil ? UserDefaults.standard.bool(forKey: "toggle-body-part-face") : true
    @State public var toggle_neck = UserDefaults.standard.object(forKey: "toggle-body-part-neck") != nil ? UserDefaults.standard.bool(forKey: "toggle-body-part-face") : true
    @State public var toggle_arms = UserDefaults.standard.bool(forKey: "toggle-body-part-arms")
    @State public var toggle_hands = UserDefaults.standard.object(forKey: "toggle-body-part-hands") != nil ? UserDefaults.standard.bool(forKey: "toggle-body-part-face") : true
    @State public var toggle_trunk = UserDefaults.standard.bool(forKey: "toggle-body-part-trunk")
    @State public var toggle_thighs = UserDefaults.standard.bool(forKey: "toggle-body-part-thighs")
    @State public var toggle_legs = UserDefaults.standard.bool(forKey: "toggle-body-part-legs")
    
    var body: some View {
        #if os(watchOS)
        VStack {
            Toggle ("face", isOn: Binding( get: { self.toggle_face }, set: {(newValue) in self.toggle_face = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_neck, forKey: "toggle-body-part-face") } ) )
            Toggle ("neck", isOn: Binding( get: { self.toggle_neck }, set: {(newValue) in self.toggle_neck = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_neck, forKey: "toggle-body-part-neck") } ) )
            Toggle ("arms", isOn: Binding( get: { self.toggle_arms }, set: {(newValue) in self.toggle_arms = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_neck, forKey: "toggle-body-part-arms") } ) )
            Toggle ("hands", isOn: Binding( get: { self.toggle_hands }, set: {(newValue) in self.toggle_hands = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_neck, forKey: "toggle-body-part-hands") } ) )
            Toggle ("torso", isOn: Binding( get: { self.toggle_trunk }, set: {(newValue) in self.toggle_trunk = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_neck, forKey: "toggle-body-part-trunk") } ) )
            Toggle ("thighs", isOn: Binding( get: { self.toggle_thighs }, set: {(newValue) in self.toggle_thighs = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_neck, forKey: "toggle-body-part-thighs") } ) )
            Toggle ("legs", isOn: Binding( get: { self.toggle_legs }, set: {(newValue) in self.toggle_legs = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_neck, forKey: "toggle-body-part-legs") } ) )
        }
        .toggleStyle(customButtonToggleStyle())
        .padding()
        .frame(maxWidth: .infinity)
        #else
        WrappingHStack{
            Toggle ("face", isOn: Binding( get: { self.toggle_face }, set: {(newValue) in self.toggle_face = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_face, forKey: "toggle-body-part-face") } ) )
                .padding(4)
            Toggle ("neck", isOn: Binding( get: { self.toggle_neck }, set: {(newValue) in self.toggle_neck = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_neck, forKey: "toggle-body-part-neck") } ) )
                .padding(4)
            Toggle ("arms", isOn: Binding( get: { self.toggle_arms }, set: {(newValue) in self.toggle_arms = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_arms, forKey: "toggle-body-part-arms") } ) )
                .padding(4)
            Toggle ("hands", isOn: Binding( get: { self.toggle_hands }, set: {(newValue) in self.toggle_hands = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_hands, forKey: "toggle-body-part-hands") } ) )
                .padding(4)
            Toggle ("torso", isOn: Binding( get: { self.toggle_trunk }, set: {(newValue) in self.toggle_trunk = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_trunk, forKey: "toggle-body-part-trunk") } ) )
                .padding(4)
            Toggle ("thighs ", isOn: Binding( get: { self.toggle_thighs }, set: {(newValue) in self.toggle_thighs = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_thighs, forKey: "toggle-body-part-thighs") } ) )
                .padding(4)
            Toggle ("legs", isOn: Binding( get: { self.toggle_legs }, set: {(newValue) in self.toggle_legs = newValue; self.update_body_exposure(); UserDefaults.standard.set(toggle_legs, forKey: "toggle-body-part-legs") } ) )
                .padding(4)
            }
            .padding()
            .toggleStyle(customButtonToggleStyle())
        #endif
        }
    
    
    func update_body_exposure()
    {
        self.body_exposed_percentage =  ( self.toggle_face ? 3.5 : 0 ) + ( self.toggle_neck ? 2.0 : 0 ) + ( self.toggle_arms ? 14.0 : 0 ) + ( self.toggle_hands ? 6.0 : 0 ) + ( self.toggle_trunk ? 26.0 : 0 ) + ( self.toggle_thighs ? 18.0 : 0 ) + ( self.toggle_legs ? 14.0 : 0)
    }
}

struct BodyPartPicker_Previews: PreviewProvider {
    @State static var exposure_percentage = 26.0
    
    static var previews: some View {
        BodyPartPicker(body_exposed_percentage: $exposure_percentage)
    }
}
