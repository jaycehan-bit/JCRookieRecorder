//
//  JCPlayerAudioRecord.swift
//  JCRookiePlayer
//
//  Created by jaycehan on 2024/8/13.
//

import AVFAudio
import Foundation

class JCPlayerAudioRecord: NSObject {

    private var audioUnit: AudioUnit?

    private func configAudioSession() -> Bool {
        do {
            // 设置缓冲区大小
            try AVAudioSession.sharedInstance().setPreferredIOBufferDuration(1.0)
            // 设置采样率（48k兼容性最佳）
            try AVAudioSession.sharedInstance().setPreferredSampleRate(JCRecordUnit.AudioRecord)
            // 设置分类（可播可录制）
            try AVAudioSession.sharedInstance().setCategory(.playback, mode: .default, options: [.allowBluetooth, .defaultToSpeaker])
            // 激活Session
            try AVAudioSession.sharedInstance().setActive(true)
            return true
        } catch {
            print("❌❌❌[JCRecordViewController] AVAudioSession Config Error")
            return false
        }
    }
    
    private func configAudioUnit() -> Bool {
        //初始化audioUnit 音频单元描述 kAudioUnitSubType_RemoteI
        var audioComponent = AudioComponentDescription()
        audioComponent.componentType = kAudioUnitType_Output
        audioComponent.componentSubType = kAudioUnitSubType_RemoteIO
        audioComponent.componentManufacturer = kAudioUnitManufacturer_Apple;
        audioComponent.componentFlags = 0;
        audioComponent.componentFlagsMask = 0;
        guard var inputComponent = AudioComponentFindNext(nil, &audioComponent) else { 
            print("❌❌❌[JCRecordViewController] configAudioUnit Error")
            return false
        }
        AudioComponentInstanceNew(inputComponent, &(self.audioUnit));
        return true
    }
    
    private func configASBD() -> Bool {
        var inputStreamDesc = AudioStreamBasicDescription()
        inputStreamDesc.mSampleRate = JCRecordUnit.AudioRecord
        inputStreamDesc.mFormatID = kAudioFormatLinearPCM
        inputStreamDesc.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonInterleaved | kAudioFormatFlagIsPacked
        inputStreamDesc.mFramesPerPacket = JCRecordUnit.FramesPrePacket
        inputStreamDesc.mChannelsPerFrame = JCRecordUnit.ChannelsPerFrame
        inputStreamDesc.mBitsPerChannel = JCRecordUnit.BitsPerChannel
        inputStreamDesc.mBytesPerFrame = JCRecordUnit.BitsPerChannel * JCRecordUnit.ChannelsPerFrame / 8
        inputStreamDesc.mBytesPerPacket = JCRecordUnit.BitsPerChannel * JCRecordUnit.ChannelsPerFrame / 8 * JCRecordUnit.FramesPrePacket
        let status = AudioUnitSetProperty(self.audioUnit!,
                                          kAudioUnitProperty_StreamFormat,
                                          kAudioUnitScope_Output,
                                          JCRecordUnit.AUInputElement,
                                          &inputStreamDesc,
                                          UInt32(MemoryLayout<AudioStreamBasicDescription>.size))
        if status != noErr {
            print("❌❌❌[JCRecordViewController] configASBD Error")
            return false
        }
        return true
    }
    
    private func configCallBack() -> Bool {
        var inputCallBack: AURenderCallbackStruct = AURenderCallbackStruct(inputProc: { inRefCon, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData -> OSStatus in
            return noErr
        }, inputProcRefCon: UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque()))
        let status = AudioUnitSetProperty(self.audioUnit!,
                                          kAudioOutputUnitProperty_SetInputCallback,
                                          kAudioUnitScope_Output,
                                          JCRecordUnit.AUInputElement,
                                          &inputCallBack,
                                          UInt32(MemoryLayout<AURenderCallbackStruct>.size))
        if status != noErr {
            print("❌❌❌[JCRecordViewController] configCallBack Error")
            return false
        }
        return true
    }
}


extension JCPlayerAudioRecord: JCRecord {
    func start() {
        AVAudioSession.sharedInstance().requestRecordPermission { granted in
            if !granted {
                return
            }
            if !self.configAudioSession() {
                return
            }
            if !self.configAudioUnit() {
                return
            }
            if !self.configASBD() {
                return
            }
            if !self.configCallBack() {
                return
            }
        }
    }
    
    func pause() {
        
    }
    
    func stop() {
        
    }
}
