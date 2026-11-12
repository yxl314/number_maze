#pragma once

/**
 * @file record_store.h
 * @brief 基于 QSettings 的最佳成绩持久化。
 */

#include <memory>

#include "domain/difficulty.h"

class QSettings;

namespace service
{

/**
 * @brief 按难度持久化最佳步数。
 *
 * 只有这一层接触 QSettings；模型保留一份内存副本，因此不依赖文件状态也能测试。
 */
class RecordStore
{
public:
    /**
     * @brief 接管一个配置实例。
     * @param settings 由本对象持有的配置实例；测试中可以为 nullptr。
     */
    explicit RecordStore(std::unique_ptr<QSettings> settings);

    /// @brief 释放配置实例。
    ~RecordStore();

    /// @brief 不可拷贝、不可赋值：配置实例唯一。
    RecordStore(const RecordStore&) = delete;
    RecordStore& operator=(const RecordStore&) = delete;

    /**
     * @brief 读某个难度的最佳步数。
     * @param level 难度等级。
     * @return 最佳步数；没有纪录时为 0。
     */
    int bestMoves(domain::Difficulty level) const;

    /**
     * @brief 写入一条最佳成绩并立即落盘。
     * @param level 难度等级。
     * @param moves 步数；非正数被忽略。
     */
    void saveBestMoves(domain::Difficulty level, int moves);

private:
    /**
     * @brief 难度在配置键中的下标。
     * @param level 难度等级。
     * @return 合法等级返回其下标；越界取值回落到 @c Easy 的下标 0。
     */
    static int levelIndex(domain::Difficulty level);

    std::unique_ptr<QSettings> m_settings;  ///< 配置存储；测试中可为空。
};

} // namespace service
