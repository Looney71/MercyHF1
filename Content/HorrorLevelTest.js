// MercyHF Horror Level Test Script
// Place this in Content/Scripts/ for testing

// Test the Index0 horror sequence
function TestHorrorSequence() {
    console.log("=== MERCYHF HORROR LEVEL TEST ===");
    console.log("Expected sequence:");
    console.log("T+0s: Level starts, main lights on");
    console.log("T+2s: BLACKOUT - main lights off, red lights intensify");
    console.log("T+2.35s: Warning text: 'DO NOT FOLLOW VOICES'");
    console.log("T+4s: Help voice plays");
    console.log("T+8s: Path reveals, text: 'VOLUNTARY ENTRY CONFIRMED'");
    console.log("=====================================");
}

// Verify required actors are tagged correctly
function VerifyActorTags() {
    const requiredTags = [
        "ENTRY_MAIN_LIGHT",
        "ENTRY_RED_LIGHT",
        "ENTRY_WARNING_TEXT",
        "ENTRY_PATH_REVEAL"
    ];

    console.log("Required actor tags for horror sequence:");
    requiredTags.forEach(tag => console.log("- " + tag));
}

// Call test functions
TestHorrorSequence();
VerifyActorTags();