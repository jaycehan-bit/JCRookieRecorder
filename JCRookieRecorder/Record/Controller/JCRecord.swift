//
//  JCRecord.swift
//  JCRookiePlayer
//
//  Created by jaycehan on 2024/8/13.
//

import Foundation

protocol JCRecord {
    /**!
     @brief 开始录制
     */
    func start()
    
    /**!
     @brief 暂停播放
    */
    func pause()
    
    /**!
     @brief 停止播放
    */
    func stop()
}
