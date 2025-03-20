#ifndef UPDATABLE_DRAWABLE_HPP
#define UPDATABLE_DRAWABLE_HPP

#include "Util/GameObject.hpp"
#include <memory>

class UpdatableDrawable : virtual public Util::GameObject { // 添加 virtual
public:
    virtual ~UpdatableDrawable() = default;
    virtual void Update() = 0; // 純虛函數，強制子類實現
    virtual std::shared_ptr<Core::Drawable> GetDrawable() const = 0; // 訪問 m_Drawable
    const glm::vec2& GetPosition() const { return m_Transform.translation; }
};

#endif // UPDATABLE_DRAWABLE_HPP