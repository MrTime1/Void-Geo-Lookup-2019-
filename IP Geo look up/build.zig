const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardReleaseOptions();

    const exe = b.addExecutable("void-geolookup", null);
    exe.setTarget(target);
    exe.setBuildMode(mode);

    exe.addCSourceFile("src/main.c", &[_][]const u8{
        "-Wall",
        "-Wextra",
    });

    exe.linkLibC();
    exe.linkSystemLibrary("wininet");
    exe.linkSystemLibrary("ws2_32");
    exe.linkSystemLibrary("iphlpapi");

    exe.install();

    const run_cmd = exe.run();
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Run the Void GeoLookup");
    run_step.dependOn(&run_cmd.step);
}
