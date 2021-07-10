//! Very basic program to test MIDI bindings of the Bela

extern crate bela_sys;
extern crate libc;
extern crate nix;

use bela_sys::midi::*;
use bela_sys::{BelaContext, BelaInitSettings};
use nix::sys::signal;
use std::{mem, ptr, thread, time};

static mut MIDI: *mut Midi = ptr::null_mut();

extern "C" fn setup(_context: *mut BelaContext, _user_data: *mut std::os::raw::c_void) -> bool {
    unsafe {
        assert!(MIDI.is_null());
        MIDI = Midi_new(b"hw:0,0,0\0".as_ptr());
    }
    true
}

extern "C" fn cleanup(_context: *mut BelaContext, _user_data: *mut std::os::raw::c_void) {
    unsafe {
        Midi_delete(MIDI);
        MIDI = ptr::null_mut();
    }
}

extern "C" fn render(_context: *mut BelaContext, _user_data: *mut std::os::raw::c_void) {
    unsafe {
        if MIDI.is_null() {
            return;
        }

        while Midi_availableMessages(MIDI) > 0 {
            let mut msg_buf = [0u8; 3];
            let msg_len = Midi_getMessage(MIDI, msg_buf.as_mut_ptr()) as usize;
            assert!(msg_len <= 3);
            if msg_len != 0 && (msg_buf[0] & 0b1111_0000) == 0b1001_0000 {
                bela_sys::rt_printf(b"Note on\n\0".as_ptr());
            }
        }
    }
}

extern "C" fn signal_handler(_signal: std::os::raw::c_int) {
    unsafe {
        bela_sys::Bela_requestStop();
    }
}

fn main() {
    unsafe {
        let mut settings = {
            let mut settings: mem::MaybeUninit<BelaInitSettings> = mem::MaybeUninit::uninit();
            bela_sys::Bela_defaultSettings(settings.as_mut_ptr());
            settings.assume_init()
        };
        bela_sys::Bela_setVerboseLevel(1);
        settings.setup = Some(setup);
        settings.render = Some(render);
        settings.cleanup = Some(cleanup);
        settings.verbose = 1;
        settings.highPerformanceMode = 1;
        settings.analogOutputsPersist = 0;
        settings.uniformSampleRate = 1;

        if bela_sys::Bela_initAudio(&mut settings, ptr::null_mut()) != 0 {
            println!("settings.render: {:?}", &settings.render);
            panic!("lol fail");
        }

        if bela_sys::Bela_startAudio() != 0 {
            println!("Unable to start real-time audio");
            bela_sys::Bela_stopAudio();
            bela_sys::Bela_cleanupAudio();
            panic!("Aborting");
        }

        let handler = signal::SigHandler::Handler(signal_handler);
        let flags = signal::SaFlags::empty();
        let set = signal::SigSet::empty();
        let sig_action = signal::SigAction::new(handler, flags, set);

        signal::sigaction(signal::SIGINT, &sig_action).unwrap();
        signal::sigaction(signal::SIGTERM, &sig_action).unwrap();

        while bela_sys::Bela_stopRequested() == 0 {
            thread::sleep(time::Duration::new(1, 0));
        }

        bela_sys::Bela_stopAudio();
        bela_sys::Bela_cleanupAudio();
    }
}
