// swift-tools-version:5.3
import PackageDescription

let package = Package(
    name: "FastRT",
    products: [
        .library(name: "FastRT", type: .dynamic, targets: ["FastRT"]),
    ],
    targets: [
        .target(
            name: "FastRT",
            resources: [
                .copy("./Resources"),
            ],
            linkerSettings: [
                .unsafeFlags(["-Xlinker", "-no_application_extension"])
            ]
        ),
    ]
    
)
