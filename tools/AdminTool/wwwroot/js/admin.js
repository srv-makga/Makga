// ── Toast notifications ───────────────────────────────────────────────────
(function () {
    // Inject container once
    const container = document.createElement('div');
    container.id = 'toastContainer';
    document.body.appendChild(container);

    window.showToast = function (message, type = 'info', duration = 3500) {
        const colors = {
            success: 'bg-success text-white',
            danger:  'bg-danger  text-white',
            warning: 'bg-warning text-dark',
            info:    'bg-info    text-dark',
        };
        const toast = document.createElement('div');
        toast.className = `toast show align-items-center border-0 shadow ${colors[type] ?? colors.info}`;
        toast.setAttribute('role', 'alert');
        toast.innerHTML = `
            <div class="d-flex">
                <div class="toast-body">${message}</div>
                <button type="button" class="btn-close btn-close-white me-2 m-auto"
                        onclick="this.closest('.toast').remove()"></button>
            </div>`;
        container.appendChild(toast);
        setTimeout(() => toast.remove(), duration);
    };
})();

// ── CSRF helper ───────────────────────────────────────────────────────────
function getCsrfToken() {
    return document.querySelector('input[name="__RequestVerificationToken"]')?.value ?? '';
}

// ── Confirm-delete forms ──────────────────────────────────────────────────
document.addEventListener('submit', function (e) {
    const form = e.target;
    if (form.dataset.confirm && !confirm(form.dataset.confirm)) {
        e.preventDefault();
    }
});
