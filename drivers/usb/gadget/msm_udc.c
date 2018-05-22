#include <common.h>
#include <usb.h>
#include <linux/usb/gadget.h>

static int msm_ep_enable(struct usb_ep *ep,
		const struct usb_endpoint_descriptor *desc)
{
	TRACE();
	return 0;
}

static int msm_ep_disable(struct usb_ep *ep)
{
	TRACE();
	return 0;
}

static int msm_ep_queue(struct usb_ep *ep,
		struct usb_request *req, gfp_t gfp_flags)
{
	TRACE();
	return 0;
}

static int msm_ep_dequeue(struct usb_ep *_ep, struct usb_request *_req)
{
	TRACE();
	return 0;
}


static struct usb_request *
msm_ep_alloc_request(struct usb_ep *ep, unsigned int gfp_flags)
{
	TRACE();
	return NULL;
}


static void msm_ep_free_request(struct usb_ep *ep, struct usb_request *req)
{
}


static struct usb_ep_ops msm_ep_ops = {
	.enable         = msm_ep_enable,
	.disable        = msm_ep_disable,
	.queue          = msm_ep_queue,
	.dequeue	= msm_ep_dequeue,
	.alloc_request  = msm_ep_alloc_request,
	.free_request   = msm_ep_free_request,
};

static int msm_pullup(struct usb_gadget *gadget, int is_on)
{
	return 0;
}

static struct usb_gadget_ops msm_udc_ops = {
	.pullup = msm_pullup,
};

static struct usb_gadget	gadget = {
		.name	= "msm_udc",
		.ops	= &msm_udc_ops,
		.is_dualspeed = 1
};


int usb_gadget_unregister_driver(struct usb_gadget_driver *driver)
{
	TRACE();
	return 0;
}

int usb_gadget_register_driver(struct usb_gadget_driver *driver)
{
	TRACE();
	int ret;

	struct ehci_ctrl * control;
	ret = usb_setup_ehci_gadget(&control);
	printf("usb_setup_ehci_gadget returns: %d\n", ret);
	return 0;
}
int usb_gadget_handle_interrupts(int index)
{
//	TRACE();
	return 1;
}

