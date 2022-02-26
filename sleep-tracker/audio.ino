int read_audio() {
    return analogRead(AUDIO_PIN);
}

int read_gate() {
    return analogRead(GATE_PIN);
}