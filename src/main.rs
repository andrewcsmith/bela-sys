extern crate libc;
extern crate nix;
extern crate bela_sys;

use bela_sys::{BelaInitSettings, BelaContext};
use std::{mem, ptr, slice, thread, time};
use nix::sys::signal;

static mut frame_index: usize = 0;

#[no_mangle]
pub unsafe extern fn setup(context: *mut BelaContext, _user_data: *mut std::os::raw::c_void) -> bool {
    let n_frames = (*context).audioFrames;
    let n_channels = (*context).audioOutChannels;

    let audio_out: &mut [f32] = slice::from_raw_parts_mut((*context).audioOut as *mut f32, (n_frames * n_channels) as usize);

    for samp in audio_out.iter_mut() {
        *samp = 0.0;
    }

    true
}

#[no_mangle]
pub unsafe extern fn render(context: *mut BelaContext, _user_data: *mut std::os::raw::c_void) {
    let n_frames = (*context).audioFrames;
    let n_channels = (*context).audioOutChannels;

    let audio_out: &mut [f32] = slice::from_raw_parts_mut((*context).audioOut as *mut f32, (n_frames * n_channels) as usize);

    let len = audio_out.len();
    for (idx, samp) in audio_out.iter_mut().enumerate() {
        *samp = (idx as f32 / len as f32) * (frame_index % 5) as f32;
    }

    frame_index += 1;
}

fn main() {
    unsafe {
        let mut settings: BelaInitSettings = mem::uninitialized();
        bela_sys::Bela_defaultSettings(&mut settings);
        bela_sys::Bela_setVerboseLevel(1);
        settings.setup = Some(setup);
        settings.render = Some(render);
        settings.cleanup = None;
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

        let handler = signal::SigHandler::Handler(bela_sys::handle_sigint);
        let flags = signal::SaFlags::empty();
        let set = signal::SigSet::empty();
        let sig_action = signal::SigAction::new(handler, flags, set);
        
        signal::sigaction(signal::SIGINT, &sig_action).unwrap();
        signal::sigaction(signal::SIGTERM, &sig_action).unwrap();

        while bela_sys::gShouldStop == 0 {
            thread::sleep(time::Duration::new(1, 0));
        }

        bela_sys::Bela_stopAudio();
        bela_sys::Bela_cleanupAudio();
    }
}
